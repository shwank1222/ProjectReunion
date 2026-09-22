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

DEFINE_LOG_CATEGORY_STATIC(LogStageExtract, Log, All);

#if WITH_EDITOR
namespace
{
	const UBoxComponent* FindBoundsBoxComponent(const AActor* Actor)
	{
		TArray<UBoxComponent*> BoxComps;
		Actor->GetComponents<UBoxComponent>(BoxComps);

		for (const UBoxComponent* Box : BoxComps)
		{
			if (Box && Box->GetName().Contains(TEXT("FloorBounds")))
			{
				return Box;
			}
		}

		return BoxComps.Num() > 0 ? BoxComps[0] : nullptr;
	}
}
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

	if (HasField(EStageExtractField::PCGParams))
	{
		if (const UBoxComponent* Bounds = FindBoundsBoxComponent(Actor))
		{
			CaptureParam(Actor, FName(*(Bounds->GetName() + TEXT(".BoxExtent"))), OutItem);
		}

		for (const FName& ParamPath : PCGParamNames)
		{
			CaptureParam(Actor, ParamPath, OutItem);
		}
	}
}

void UStageExtractRule::CaptureParam(const AActor* Actor, FName ParamPath, FStageExtractedItem& OutItem) const
{
	FName PropertyName = NAME_None;
	const UObject* Target = ResolveParamTarget(Actor, ParamPath, PropertyName);
	if (!Target)
	{
		UE_LOG(LogStageExtract, Warning, TEXT("[%s] PCGParam '%s': target not found on %s"),
			*RuleId.ToString(), *ParamPath.ToString(), *Actor->GetName());
		return;
	}

	const FProperty* Prop = Target->GetClass()->FindPropertyByName(PropertyName);
	if (!Prop)
	{
		UE_LOG(LogStageExtract, Warning, TEXT("[%s] PCGParam '%s': property '%s' not found on %s"),
			*RuleId.ToString(), *ParamPath.ToString(), *PropertyName.ToString(), *Target->GetName());
		return;
	}

	FString ValueText;
	Prop->ExportText_InContainer(0, ValueText, Target, nullptr, nullptr, PPF_None);
	OutItem.PCGParams.Add(ParamPath, ValueText);
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
	ApplyPCGParams(Spawned, Item);
	RegeneratePCG(Spawned, Ctx);

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

void UStageExtractRule::ApplyPCGParams(AActor* SpawnedActor, const FStageExtractedItem& Item) const
{
	if (!SpawnedActor || !HasField(EStageExtractField::PCGParams))
	{
		return;
	}

	for (const TPair<FName, FString>& Param : Item.PCGParams)
	{
		FName PropertyName = NAME_None;
		UObject* Target = ResolveParamTarget(SpawnedActor, Param.Key, PropertyName);
		if (!Target)
		{
			UE_LOG(LogStageExtract, Warning, TEXT("[%s] PCGParam '%s': target not found on %s"),
				*RuleId.ToString(), *Param.Key.ToString(), *SpawnedActor->GetName());
			continue;
		}

		const FProperty* Prop = Target->GetClass()->FindPropertyByName(PropertyName);
		if (!Prop)
		{
			UE_LOG(LogStageExtract, Warning, TEXT("[%s] PCGParam '%s': property '%s' not found on %s"),
				*RuleId.ToString(), *Param.Key.ToString(), *PropertyName.ToString(), *Target->GetName());
			continue;
		}

		void* ValuePtr = Prop->ContainerPtrToValuePtr<void>(Target);
		if (!Prop->ImportText_Direct(*Param.Value, ValuePtr, Target, PPF_None))
		{
			UE_LOG(LogStageExtract, Warning, TEXT("[%s] PCGParam '%s': failed to import '%s'"),
				*RuleId.ToString(), *Param.Key.ToString(), *Param.Value);
			continue;
		}

		RefreshAfterParamChange(Target);
	}

	SpawnedActor->RerunConstructionScripts();
	SpawnedActor->SetActorTransform(Item.Transform);
}

void UStageExtractRule::RegeneratePCG(AActor* SpawnedActor, FStageSpawnContext& Ctx) const
{
	if (!SpawnedActor || !HasField(EStageExtractField::PCGParams))
	{
		return;
	}

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

UObject* UStageExtractRule::ResolveParamTarget(const AActor* Actor, FName ParamPath, FName& OutPropertyName)
{
	OutPropertyName = NAME_None;

	if (!Actor || ParamPath == NAME_None)
	{
		return nullptr;
	}

	FString ComponentName;
	FString PropertyName;
	if (!ParamPath.ToString().Split(TEXT("."), &ComponentName, &PropertyName))
	{
		OutPropertyName = ParamPath;
		return const_cast<AActor*>(Actor);
	}

	OutPropertyName = FName(*PropertyName);

	TArray<UActorComponent*> Components;
	Actor->GetComponents(Components);

	for (UActorComponent* Comp : Components)
	{
		if (Comp && Comp->GetName() == ComponentName)
		{
			return Comp;
		}
	}

	for (UActorComponent* Comp : Components)
	{
		if (!Comp)
		{
			continue;
		}

		const FString InstanceName = Comp->GetName();
		if (InstanceName.Contains(ComponentName) || ComponentName.Contains(InstanceName))
		{
			return Comp;
		}
	}

	return nullptr;
}

void UStageExtractRule::RefreshAfterParamChange(UObject* Target)
{
	if (UBoxComponent* Box = Cast<UBoxComponent>(Target))
	{
		Box->SetBoxExtent(Box->GetUnscaledBoxExtent(), true);
		return;
	}

	if (USceneComponent* SceneComp = Cast<USceneComponent>(Target))
	{
		SceneComp->UpdateBounds();
		SceneComp->MarkRenderStateDirty();
	}
}
