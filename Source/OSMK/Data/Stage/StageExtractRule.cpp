#include "Data/Stage/StageExtractRule.h"
#include "Data/Stage/StageExtractedCache.h"
#include "Data/Stage/StageExtractPCGWaiter.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMesh.h"
#include "PCGComponent.h"

#if WITH_EDITOR
#include "EngineUtils.h"
#endif

#if WITH_EDITOR
void UStageExtractRule::ExtractFromWorld(UWorld* World, FName LevelName, UStageExtractedCache* Cache) const
{
	if (!World || !Cache || !TargetClass || RuleId == NAME_None)
	{
		return;
	}

	FStageExtractedGroup Group;

	for (TActorIterator<AActor> It(World, TargetClass); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || !MatchesActor(Actor))
		{
			continue;
		}

		if (bMatchExactClass && Actor->GetClass() != TargetClass)
		{
			continue;
		}

		FStageExtractedItem Item;
		FillItem(Actor, Item);
		Group.Items.Add(Item);
	}

	Cache->Write(RuleId, LevelName, Group);
}

bool UStageExtractRule::MatchesActor(const AActor* Actor) const
{
	return Actor != nullptr;
}

void UStageExtractRule::FillItem(const AActor* Actor, FStageExtractedItem& OutItem) const
{
	if (!Actor)
	{
		return;
	}

	if (HasField(EStageExtractField::Class))
	{
		OutItem.ActorClass = TSoftClassPtr<AActor>(Actor->GetClass());
	}

	if (HasField(EStageExtractField::Transform))
	{
		OutItem.Transform = Actor->GetActorTransform();
	}

	if (HasField(EStageExtractField::Mesh) || HasField(EStageExtractField::Collision) || HasField(EStageExtractField::Materials))
	{
		if (UStaticMeshComponent* SMComp = Actor->FindComponentByClass<UStaticMeshComponent>())
		{
			if (HasField(EStageExtractField::Mesh))
			{
				OutItem.Mesh = TSoftObjectPtr<UStaticMesh>(SMComp->GetStaticMesh());
			}

			if (HasField(EStageExtractField::Collision))
			{
				OutItem.CollisionProfile = SMComp->GetCollisionProfileName();
			}

			if (HasField(EStageExtractField::Materials))
			{
				const int32 MatCount = SMComp->GetNumMaterials();
				for (int32 i = 0; i < MatCount; ++i)
				{
					OutItem.Materials.Add(TSoftObjectPtr<UMaterialInterface>(SMComp->GetMaterial(i)));
				}
			}
		}
	}

	if (HasField(EStageExtractField::PCGExtent))
	{
		TArray<UBoxComponent*> BoxComps;
		Actor->GetComponents<UBoxComponent>(BoxComps);

		UBoxComponent* Target = nullptr;
		for (UBoxComponent* Box : BoxComps)
		{
			if (Box && Box->GetName().Contains(TEXT("FloorBounds")))
			{
				Target = Box;
				break;
			}
		}
		if (!Target && BoxComps.Num() > 0)
		{
			Target = BoxComps[0];
		}

		if (Target)
		{
			OutItem.PCGExtent = Target->GetUnscaledBoxExtent();
		}
	}
}
#endif

void UStageExtractRule::SpawnStage(FStageSpawnContext& Ctx) const
{
	if (!Ctx.World || !Ctx.Cache || RuleId == NAME_None)
	{
		return;
	}

	const FStageExtractedGroup* Group = Ctx.Cache->Read(RuleId, Ctx.LevelName);
	if (!Group)
	{
		return;
	}

	for (const FStageExtractedItem& Item : Group->Items)
	{
		SpawnItem(Ctx, Item);
	}
}

void UStageExtractRule::SpawnItem(FStageSpawnContext& Ctx, const FStageExtractedItem& Item) const
{
	if (!Ctx.World)
	{
		return;
	}

	UClass* ClassToSpawn = nullptr;
	if (HasField(EStageExtractField::Class))
	{
		ClassToSpawn = Item.ActorClass.LoadSynchronous();
	}
	if (!ClassToSpawn && TargetClass)
	{
		ClassToSpawn = TargetClass.Get();
	}
	if (!ClassToSpawn)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* Spawned = Ctx.World->SpawnActor<AActor>(ClassToSpawn, Item.Transform, SpawnParams);
	if (!Spawned)
	{
		return;
	}

	ConfigureSpawnedActor(Spawned, Item, Ctx);
	ApplyPCGExtent(Spawned, Item, Ctx);

	if (Ctx.TrackActor)
	{
		Ctx.TrackActor(Spawned);
	}
}

void UStageExtractRule::ConfigureSpawnedActor(AActor* SpawnedActor, const FStageExtractedItem& Item, FStageSpawnContext& Ctx) const
{
	if (!SpawnedActor)
	{
		return;
	}

	const bool bNeedsMesh = HasField(EStageExtractField::Mesh);
	const bool bNeedsCollision = HasField(EStageExtractField::Collision);
	const bool bNeedsMaterials = HasField(EStageExtractField::Materials);

	if (!bNeedsMesh && !bNeedsCollision && !bNeedsMaterials)
	{
		return;
	}

	UStaticMeshComponent* SMComp = SpawnedActor->FindComponentByClass<UStaticMeshComponent>();
	if (!SMComp)
	{
		return;
	}

	if (bNeedsMesh)
	{
		if (UStaticMesh* Mesh = Item.Mesh.LoadSynchronous())
		{
			SpawnedActor->SetActorScale3D(Item.Transform.GetScale3D());
			SMComp->SetMobility(EComponentMobility::Movable);
			SMComp->SetStaticMesh(Mesh);
		}
	}

	if (bNeedsCollision && Item.CollisionProfile != NAME_None)
	{
		SMComp->SetCollisionProfileName(Item.CollisionProfile);
	}

	if (bNeedsMaterials)
	{
		for (int32 i = 0; i < Item.Materials.Num(); ++i)
		{
			if (UMaterialInterface* Mat = Item.Materials[i].LoadSynchronous())
			{
				SMComp->SetMaterial(i, Mat);
			}
		}
	}
}

void UStageExtractRule::ApplyPCGExtent(AActor* SpawnedActor, const FStageExtractedItem& Item, FStageSpawnContext& Ctx) const
{
	if (!SpawnedActor || !HasField(EStageExtractField::PCGExtent))
	{
		return;
	}

	TArray<UBoxComponent*> BoxComps;
	SpawnedActor->GetComponents<UBoxComponent>(BoxComps);

	UBoxComponent* Target = nullptr;
	for (UBoxComponent* Box : BoxComps)
	{
		if (Box && Box->GetName().Contains(TEXT("FloorBounds")))
		{
			Target = Box;
			break;
		}
	}
	if (!Target && BoxComps.Num() > 0)
	{
		Target = BoxComps[0];
	}

	if (!Target)
	{
		return;
	}

	Target->SetBoxExtent(Item.PCGExtent, true);

	SpawnedActor->RerunConstructionScripts();
	SpawnedActor->SetActorTransform(Item.Transform);

	TArray<UPCGComponent*> PCGComps;
	SpawnedActor->GetComponents<UPCGComponent>(PCGComps);

	for (UPCGComponent* PCG : PCGComps)
	{
		if (!PCG)
		{
			continue;
		}

		if (Ctx.BeginAsync)
		{
			Ctx.BeginAsync();
		}

		UStageExtractPCGWaiter* Waiter = NewObject<UStageExtractPCGWaiter>(SpawnedActor);
		Waiter->AddToRoot();
		Waiter->OnComplete = Ctx.EndAsync;
		PCG->OnPCGGraphGeneratedDelegate.AddUObject(Waiter, &UStageExtractPCGWaiter::HandlePCGGenerated);

		PCG->GenerateLocal(true);
	}
}
