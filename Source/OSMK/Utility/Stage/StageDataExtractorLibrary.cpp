#include "StageDataExtractorLibrary.h"
#include "Engine/DataTable.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Data/Stage/StageStaticMeshData.h"
#include "Data/Stage/StageEnemyData.h"
#include "Character/AI/EnemyCharacter.h"
#include "Data/Stage/StageGimmickData.h"
#include "Interactable/Gimmick/GimmickBase.h"
#include "Data/Stage/StageActorData.h"
#include "Data/Stage/StageScoutCameraData.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/TriggerBox.h"
#include "Engine/TriggerCapsule.h"
#include "Engine/TriggerSphere.h"
#include "Engine/TriggerVolume.h"

#if WITH_EDITOR
#include "EngineUtils.h"
#endif

void UStageDataExtractorLibrary::ExtractStaticMeshFromLevels(UDataTable* TargetDataTable, TArray<TSoftObjectPtr<UWorld>> TargetLevels)
{
#if WITH_EDITOR
	if (!TargetDataTable)
	{
		return;
	}

	TargetDataTable->EmptyTable();

	for (const TSoftObjectPtr<UWorld>& SoftWorld : TargetLevels)
	{
		if (SoftWorld.IsNull())
		{
			continue;
		}

		UWorld* LoadedWorld = SoftWorld.LoadSynchronous();
		if (!LoadedWorld)
		{
			continue;
		}

		FName RowName = FName(*LoadedWorld->GetName());
		FStageStaticMeshData NewStageData;

		for (TActorIterator<AStaticMeshActor> It(LoadedWorld); It; ++It)
		{
			AStaticMeshActor* SMActor = *It;
			if (!SMActor)
			{
				continue;
			}

			UStaticMeshComponent* SMComp = SMActor->GetStaticMeshComponent();
			if (!SMComp || !SMComp->GetStaticMesh())
			{
				continue;
			}

			FStageStaticMeshItem ItemData;
			ItemData.StaticMesh = TSoftObjectPtr<UStaticMesh>(SMComp->GetStaticMesh());
			ItemData.Transform = SMActor->GetActorTransform();
			ItemData.CollisionProfileName = SMComp->GetCollisionProfileName();

			int32 MatCount = SMComp->GetNumMaterials();
			for (int32 i = 0; i < MatCount; i++)
			{
				UMaterialInterface* Mat = SMComp->GetMaterial(i);
				ItemData.Materials.Add(TSoftObjectPtr<UMaterialInterface>(Mat));
			}

			NewStageData.MeshList.Add(ItemData);
		}

		TargetDataTable->AddRow(RowName, NewStageData);
	}

	TargetDataTable->MarkPackageDirty();
#endif
}

void UStageDataExtractorLibrary::ExtractEnemyFromLevels(UDataTable* TargetDataTable, TArray<TSoftObjectPtr<UWorld>> TargetLevels)
{
#if WITH_EDITOR
	if (!TargetDataTable)
	{
		return;
	}

	TargetDataTable->EmptyTable();

	for (const TSoftObjectPtr<UWorld>& SoftWorld : TargetLevels)
	{
		if (SoftWorld.IsNull())
		{
			continue;
		}

		UWorld* LoadedWorld = SoftWorld.LoadSynchronous();
		if (!LoadedWorld)
		{
			continue;
		}

		FName RowName = FName(*LoadedWorld->GetName());
		FStageEnemyData NewEnemyData;

		for (TActorIterator<AEnemyCharacter> It(LoadedWorld); It; ++It)
		{
			AEnemyCharacter* Enemy = *It;
			if (!Enemy)
			{
				continue;
			}

			FStageEnemyItem Item;
			Item.Location = Enemy->GetActorLocation();
			Item.Rotation = Enemy->GetActorRotation();
			NewEnemyData.EnemyList.Add(Item);
		}

		TargetDataTable->AddRow(RowName, NewEnemyData);
	}

	TargetDataTable->MarkPackageDirty();
#endif
}

void UStageDataExtractorLibrary::ExtractGimmickFromLevels(UDataTable* TargetDataTable, TArray<TSoftObjectPtr<UWorld>> TargetLevels)
{
#if WITH_EDITOR
	if (!TargetDataTable)
	{
		return;
	}

	TargetDataTable->EmptyTable();

	for (const TSoftObjectPtr<UWorld>& SoftWorld : TargetLevels)
	{
		if (SoftWorld.IsNull())
		{
			continue;
		}

		UWorld* LoadedWorld = SoftWorld.LoadSynchronous();
		if (!LoadedWorld)
		{
			continue;
		}

		FName RowName = FName(*LoadedWorld->GetName());
		FStageGimmickData NewGimmickData;

		for (TActorIterator<AGimmickBase> It(LoadedWorld); It; ++It)
		{
			AGimmickBase* Gimmick = *It;
			if (!Gimmick)
			{
				continue;
			}

			FStageGimmickItem Item;
			Item.GimmickClass = TSoftClassPtr<AGimmickBase>(Gimmick->GetClass());
			Item.Transform = Gimmick->GetActorTransform();
			NewGimmickData.GimmickList.Add(Item);
		}

		TargetDataTable->AddRow(RowName, NewGimmickData);
	}

	TargetDataTable->MarkPackageDirty();
#endif
}

void UStageDataExtractorLibrary::ExtractActorDataFromLevels(UDataTable* TargetDataTable, TArray<TSoftObjectPtr<UWorld>> TargetLevels)
{
#if WITH_EDITOR
	if (!TargetDataTable)
	{
		return;
	}

	TargetDataTable->EmptyTable();

	for (const TSoftObjectPtr<UWorld>& SoftWorld : TargetLevels)
	{
		if (SoftWorld.IsNull())
		{
			continue;
		}

		UWorld* LoadedWorld = SoftWorld.LoadSynchronous();
		if (!LoadedWorld)
		{
			continue;
		}

		FName RowName = FName(*LoadedWorld->GetName());
		FStageActorData NewActorData;

		for (TActorIterator<AActor> It(LoadedWorld); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor)
			{
				continue;
			}

			if (Cast<ATriggerBox>(Actor) || Cast<ATriggerCapsule>(Actor) ||
				Cast<ATriggerSphere>(Actor) || Cast<ATriggerVolume>(Actor))
			{
				FStageActorItem Item;
				Item.ActorClass = TSoftClassPtr<AActor>(Actor->GetClass());
				Item.Transform = Actor->GetActorTransform();
				NewActorData.TriggerList.Add(Item);
				continue;
			}

			if (Cast<APlayerStart>(Actor))
			{
				NewActorData.PlayerStartTransform = Actor->GetActorTransform();
			}
		}

		TargetDataTable->AddRow(RowName, NewActorData);
	}

	TargetDataTable->MarkPackageDirty();
#endif
}

void UStageDataExtractorLibrary::ExtractScoutCameraDataFromLevels(UDataTable* TargetDataTable, TArray<TSoftObjectPtr<UWorld>> TargetLevels)
{
#if WITH_EDITOR
	if (!TargetDataTable)
	{
		return;
	}

	TargetDataTable->EmptyTable();

	for (const TSoftObjectPtr<UWorld>& SoftWorld : TargetLevels)
	{
		if (SoftWorld.IsNull())
		{
			continue;
		}

		UWorld* LoadedWorld = SoftWorld.LoadSynchronous();
		if (!LoadedWorld)
		{
			continue;
		}

		FName RowName = FName(*LoadedWorld->GetName());
		FStageScoutCameraData NewCameraData;

		for (TActorIterator<AActor> It(LoadedWorld); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor)
			{
				continue;
			}

			if (USpringArmComponent* SpringArm = Actor->FindComponentByClass<USpringArmComponent>())
			{
				NewCameraData.CameraTransform = Actor->GetActorTransform();
				NewCameraData.SpringArmLength = SpringArm->TargetArmLength;
				NewCameraData.SpringArmSocketOffset = SpringArm->SocketOffset;
				break;
			}
		}

		TargetDataTable->AddRow(RowName, NewCameraData);
	}

	TargetDataTable->MarkPackageDirty();
#endif
}