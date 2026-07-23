#include "GameMode/OSMKInGameGameMode.h"
#include "UI/Scouting/ScoutingWidget.h"
#include "Data/StageData.h"
#include "Blueprint/UserWidget.h"

void AOSMKInGameGameMode::BeginPlay()
{
	Super::BeginPlay();

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

void AOSMKInGameGameMode::SpawnEnemies(int32 StageIndex)
{
	if (!StageData)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] StageData is null"));
		return;
	}

	if (!StageData->Stages.IsValidIndex(StageIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] Invalid StageIndex: %d"), StageIndex);
		return;
	}

	UClass* EnemyClass = StageData->EnemyClass.LoadSynchronous();
	if (!EnemyClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] EnemyClass is null"));
		return;
	}

	const TArray<FVector>& SpawnLocations = StageData->Stages[StageIndex].EnemySpawnLocations;
	UWorld* World = GetWorld();

	for (const FVector& Location : SpawnLocations)
	{
		World->SpawnActor<AActor>(EnemyClass, Location, FRotator::ZeroRotator);
	}

	UE_LOG(LogTemp, Log, TEXT("[InGameGameMode] Spawned %d enemies"), SpawnLocations.Num());
}
