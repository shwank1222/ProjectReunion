#include "GameMode/OSMKInGameGameMode.h"
#include "UI/Scouting/ScoutingWidget.h"
#include "Core/OSMKGameState.h"
#include "Data/StageData.h"
#include "Data/Stage/StageStaticMeshData.h"
#include "Data/Stage/StageEnemyData.h"
#include "Character/AI/EnemyCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"

void AOSMKInGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnStage(0);

	if (!ScoutingWidgetClass)
	{
		return;
	}

	ScoutingWidget = CreateWidget<UScoutingWidget>(GetWorld(), ScoutingWidgetClass);
	if (ScoutingWidget)
	{
		ScoutingWidget->AddToViewport();
	}
}

void AOSMKInGameGameMode::SpawnStage(int32 StageIndex)
{
	if (StageIndex > 0)
	{
		ClearStage();
	}

	SpawnStaticMesh(StageIndex);
	SpawnEnemies(StageIndex);
}

void AOSMKInGameGameMode::SpawnStaticMesh(int32 StageIndex)
{
	if (!StageData || !StageData->StageStaticMeshData)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] SpawnStage: StageData or StageStaticMeshData is null"));
		return;
	}

	TArray<FName> RowNames = StageData->StageStaticMeshData->GetRowNames();
	if (!RowNames.IsValidIndex(StageIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] SpawnStage: invalid StageIndex %d"), StageIndex);
		return;
	}

	FStageStaticMeshData* Row = StageData->StageStaticMeshData->FindRow<FStageStaticMeshData>(RowNames[StageIndex], TEXT(""));
	if (!Row)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] SpawnStage: row not found"));
		return;
	}

	UWorld* World = GetWorld();

	for (const FStageStaticMeshItem& Item : Row->MeshList)
	{
		UStaticMesh* Mesh = Item.StaticMesh.LoadSynchronous();
		if (!Mesh)
		{
			continue;
		}

		AStaticMeshActor* MeshActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Item.Transform);
		if (!MeshActor)
		{
			continue;
		}

		UStaticMeshComponent* MeshComp = MeshActor->GetStaticMeshComponent();
		MeshActor->SetMobility(EComponentMobility::Movable);
		MeshComp->SetStaticMesh(Mesh);
		MeshComp->SetCollisionProfileName(Item.CollisionProfileName);

		for (int32 i = 0; i < Item.Materials.Num(); i++)
		{
			UMaterialInterface* Mat = Item.Materials[i].LoadSynchronous();
			if (Mat)
			{
				MeshComp->SetMaterial(i, Mat);
			}
		}

		SpawnedMeshActors.Add(MeshActor);
	}
	UE_LOG(LogTemp, Log, TEXT("[InGameGameMode] Spawned %d static meshes for stage %d"), SpawnedMeshActors.Num(), StageIndex);
}

void AOSMKInGameGameMode::ClearStage()
{
	ClearStaticMesh();
	ClearEnemies();
	UE_LOG(LogTemp, Log, TEXT("[InGameGameMode] Stage cleared"));
}

void AOSMKInGameGameMode::ClearStaticMesh()
{
	for (AActor* Actor : SpawnedMeshActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	SpawnedMeshActors.Empty();
}

void AOSMKInGameGameMode::ClearEnemies()
{
	for (AActor* Actor : SpawnedEnemyActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	SpawnedEnemyActors.Empty();
}

void AOSMKInGameGameMode::SpawnEnemies(int32 StageIndex)
{
	if (!StageData || !StageData->StageEnemyData)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] SpawnEnemies: StageData or StageEnemyData is null"));
		return;
	}

	UClass* EnemyClass = StageData->EnemyClass.LoadSynchronous();
	if (!EnemyClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] SpawnEnemies: EnemyClass is null"));
		return;
	}

	TArray<FName> RowNames = StageData->StageEnemyData->GetRowNames();
	if (!RowNames.IsValidIndex(StageIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("[InGameGameMode] SpawnEnemies: invalid StageIndex %d"), StageIndex);
		return;
	}

	FStageEnemyData* Row = StageData->StageEnemyData->FindRow<FStageEnemyData>(RowNames[StageIndex], TEXT(""));
	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InGameGameMode] SpawnEnemies: row not found"));
		return;
	}

	UWorld* World = GetWorld();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (const FStageEnemyItem& Item : Row->EnemyList)
	{
		FTransform SpawnTransform(Item.Rotation, Item.Location);
		AActor* SpawnedActor = World->SpawnActor<AActor>(EnemyClass, SpawnTransform, SpawnParams);
		if (SpawnedActor)
		{
			SpawnedEnemyActors.Add(SpawnedActor);
		}
	}

	if (AOSMKGameState* GS = GetGameState<AOSMKGameState>())
	{
		GS->SetEnemyCount(SpawnedEnemyActors.Num());
	}

	UE_LOG(LogTemp, Log, TEXT("[InGameGameMode] Spawned %d enemies for stage %d"), SpawnedEnemyActors.Num(), StageIndex);
}
