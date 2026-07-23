#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "OSMKInGameGameMode.generated.h"

UCLASS()
class OSMK_API AOSMKInGameGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SpawnEnemies(int32 StageIndex);

	UFUNCTION(BlueprintCallable)
	void SpawnStage(int32 StageIndex);

	UFUNCTION(BlueprintCallable)
	void ClearStage();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UScoutingWidget> ScoutingWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Stage")
	class UStageData* StageData = nullptr;

private:
	UPROPERTY()
	class UScoutingWidget* ScoutingWidget = nullptr;

	UPROPERTY()
	TArray<AActor*> SpawnedMeshActors;

	UPROPERTY()
	TArray<AActor*> SpawnedEnemyActors;
};
