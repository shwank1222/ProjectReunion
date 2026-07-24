#include "GameMode/OSMKInGameGameMode.h"

#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"
#include "Character/OSMKPlayerController.h"
#include "UI/Scouting/ScoutingWidget.h"
#include "Core/OSMKGameState.h"
#include "Data/StageData.h"
#include "Data/Stage/StageStaticMeshData.h"
#include "Data/Stage/StageEnemyData.h"
#include "Data/Stage/StageGimmickData.h"
#include "Data/Stage/StageActorData.h"
#include "Data/Stage/StageScoutCameraData.h"
#include "Character/AI/EnemyCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/StaticMeshActor.h"
#include "NavMesh/NavMeshBoundsVolume.h"

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
	SpawnGimmicks(StageIndex);
	SpawnActors(StageIndex);
	SpawnScoutCamera(StageIndex);
	SpawnPlayerCharacter();
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

void AOSMKInGameGameMode::SpawnGimmicks(int32 StageIndex)
{
	if (!StageData || !StageData->StageGimmickData)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] SpawnGimmicks: StageData or StageGimmickData is null"));
		return;
	}

	TArray<FName> RowNames = StageData->StageGimmickData->GetRowNames();
	if (!RowNames.IsValidIndex(StageIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("[InGameGameMode] SpawnGimmicks: invalid StageIndex %d"), StageIndex);
		return;
	}

	FStageGimmickData* Row = StageData->StageGimmickData->FindRow<FStageGimmickData>(RowNames[StageIndex], TEXT(""));
	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InGameGameMode] SpawnGimmicks: row not found"));
		return;
	}

	UWorld* World = GetWorld();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (const FStageGimmickItem& Item : Row->GimmickList)
	{
		UClass* GimmickClass = Item.GimmickClass.LoadSynchronous();
		if (!GimmickClass)
		{
			continue;
		}

		AActor* SpawnedActor = World->SpawnActor<AActor>(GimmickClass, Item.Transform, SpawnParams);
		if (SpawnedActor)
		{
			SpawnedGimmickActors.Add(SpawnedActor);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[InGameGameMode] Spawned %d gimmicks for stage %d"), SpawnedGimmickActors.Num(), StageIndex);
}

void AOSMKInGameGameMode::ClearStage()
{
	ClearStaticMesh();
	ClearEnemies();
	ClearGimmicks();
	ClearActors();
	ClearScoutCamera();
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

void AOSMKInGameGameMode::ClearGimmicks()
{
	for (AActor* Actor : SpawnedGimmickActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	SpawnedGimmickActors.Empty();
}

void AOSMKInGameGameMode::SpawnActors(int32 StageIndex)
{
	if (!StageData || !StageData->StageActorData)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] SpawnActors: StageData or StageActorData is null"));
		return;
	}

	TArray<FName> RowNames = StageData->StageActorData->GetRowNames();
	if (!RowNames.IsValidIndex(StageIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("[InGameGameMode] SpawnActors: invalid StageIndex %d"), StageIndex);
		return;
	}

	FStageActorData* Row = StageData->StageActorData->FindRow<FStageActorData>(RowNames[StageIndex], TEXT(""));
	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InGameGameMode] SpawnActors: row not found"));
		return;
	}

	PlayerStartTransform = Row->PlayerStartTransform;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (const FStageActorItem& Item : Row->TriggerList)
	{
		UClass* TriggerClass = Item.ActorClass.LoadSynchronous();
		if (!TriggerClass)
		{
			continue;
		}

		AActor* Spawned = GetWorld()->SpawnActor<AActor>(TriggerClass, Item.Transform, SpawnParams);
		if (Spawned)
		{
			SpawnedTriggerActors.Add(Spawned);
		}
	}
	
	for (const FStageNavMeshItem& Item : Row->NavMeshList)
	{
		ANavMeshBoundsVolume* NavMeshVol = GetWorld()->SpawnActor<ANavMeshBoundsVolume>(ANavMeshBoundsVolume::StaticClass(), Item.Transform, SpawnParams);
		if (NavMeshVol)
		{
			SpawnedTriggerActors.Add(NavMeshVol);
		}
	}

	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		NavSys->Build();
	}
}

void AOSMKInGameGameMode::SpawnScoutCamera(int32 StageIndex)
{
	if (!StageData || !StageData->StageScoutCameraData)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] SpawnScoutCamera: StageData or StageScoutCameraData is null"));
		return;
	}

	UClass* CameraClass = StageData->ScoutCameraClass.LoadSynchronous();
	if (!CameraClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] SpawnScoutCamera: ScoutCameraClass is null"));
		return;
	}

	TArray<FName> RowNames = StageData->StageScoutCameraData->GetRowNames();
	if (!RowNames.IsValidIndex(StageIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("[InGameGameMode] SpawnScoutCamera: invalid StageIndex %d"), StageIndex);
		return;
	}

	FStageScoutCameraData* Row = StageData->StageScoutCameraData->FindRow<FStageScoutCameraData>(RowNames[StageIndex], TEXT(""));
	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InGameGameMode] SpawnScoutCamera: row not found"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnedScoutCameraActor = GetWorld()->SpawnActor<AActor>(CameraClass, Row->CameraTransform, SpawnParams);
	if (SpawnedScoutCameraActor)
	{
		if (USpringArmComponent* SpringArm = SpawnedScoutCameraActor->FindComponentByClass<USpringArmComponent>())
		{
			SpringArm->TargetArmLength = Row->SpringArmLength;
			SpringArm->SocketOffset = Row->SpringArmSocketOffset;
		}

		if (AOSMKPlayerController* PC = Cast<AOSMKPlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			PC->SetViewTarget(SpawnedScoutCameraActor);
			PC->EnterScoutingMode(SpawnedScoutCameraActor);
		}
	}
}

void AOSMKInGameGameMode::ClearActors()
{
	for (AActor* Actor : SpawnedTriggerActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	SpawnedTriggerActors.Empty();
}

void AOSMKInGameGameMode::ClearScoutCamera()
{
	if (IsValid(SpawnedScoutCameraActor))
	{
		SpawnedScoutCameraActor->Destroy();
		SpawnedScoutCameraActor = nullptr;
	}
}

void AOSMKInGameGameMode::SpawnPlayerCharacter()
{
	if (!StageData)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] SpawnPlayerCharacter: StageData is null"));
		return;
	}

	UClass* PlayerCharacterClass = StageData->PlayerCharacterClass.LoadSynchronous();
	if (!PlayerCharacterClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] SpawnPlayerCharacter: PlayerCharacterClass is null"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	SpawnedPlayerCharacter = GetWorld()->SpawnActor<APawn>(PlayerCharacterClass, PlayerStartTransform, SpawnParams);
}

void AOSMKInGameGameMode::PossessPlayerCharacter()
{
	if (!SpawnedPlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] PossessPlayerCharacter: SpawnedPlayerCharacter is null"));
		return;
	}

	AOSMKPlayerController* PC = Cast<AOSMKPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC)
	{
		return;
	}

	PC->ExitScoutingMode();
	PC->Possess(SpawnedPlayerCharacter);

	if (ScoutingWidget)
	{
		ScoutingWidget->RemoveFromParent();
		ScoutingWidget = nullptr;
	}
}

void AOSMKInGameGameMode::ActivateEnemies()
{
	UE_LOG(LogTemp, Log, TEXT("[InGameGameMode] ActivateEnemies called. Total spawned enemies: %d"), SpawnedEnemyActors.Num());

	for (AActor* Actor : SpawnedEnemyActors)
	{
		if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Actor))
		{
			Enemy->ActivateEnemy();
			UE_LOG(LogTemp, Log, TEXT("[InGameGameMode] Activated Enemy: %s"), *Enemy->GetName());
		}
	}
}