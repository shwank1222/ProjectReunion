#pragma once

#include "CoreMinimal.h"
#include "Data/StageData.h"
#include "GameFramework/GameMode.h"
#include "OSMKInGameGameMode.generated.h"

UCLASS()
class OSMK_API AOSMKInGameGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SpawnStage(int32 StageIndex);
	
	UFUNCTION(BlueprintCallable)
	void SpawnEnemies(int32 StageIndex);
	
	UFUNCTION(BlueprintCallable)
	void SpawnStaticMesh(int32 StageIndex);
	
	UFUNCTION(BlueprintCallable)
	void SpawnGimmicks(int32 StageIndex);

	UFUNCTION(BlueprintCallable)
	void ClearStage();
	
	UFUNCTION(BlueprintCallable)
	void ClearStaticMesh();
	
	UFUNCTION(BlueprintCallable)
	void ClearEnemies();
	
	UFUNCTION(BlueprintCallable)
	void ClearGimmicks();

	UFUNCTION(BlueprintCallable)
	void SpawnActors(int32 StageIndex);

	UFUNCTION(BlueprintCallable)
	void SpawnScoutCamera(int32 StageIndex);

	UFUNCTION(BlueprintCallable)
	void ClearActors();

	UFUNCTION(BlueprintCallable)
	void ClearScoutCamera();

	UFUNCTION(BlueprintCallable)
	void ClearPlayerCharacter();

	FTransform GetPlayerStartTransform() const { return PlayerStartTransform; }

	const TMap<FName, int32>* GetCurrentStageBulletCounts() const
	{
		if (StageData && StageData->StageConfigs.IsValidIndex(CurrentStageIndex))
		{
			return &StageData->StageConfigs[CurrentStageIndex].BulletCounts;
		}
		return nullptr;
	}

	UFUNCTION(BlueprintCallable)
	void SpawnPlayerCharacter();

	UFUNCTION(BlueprintCallable)
	void PossessPlayerCharacter();

	UFUNCTION(BlueprintCallable)
	void ActivateEnemies();

	UFUNCTION()
	void HandleStageClear();

	UFUNCTION()
	void HandleStageFail();

	UFUNCTION(BlueprintCallable)
	void RetryStage();

protected:
	virtual void BeginPlay() override;

private:
	void ShowStageClearWidget();
	void ShowStageFailWidget();
	void ProceedToNextStage();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UScoutingWidget> ScoutingWidgetClass = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> StageClearWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> StageFailWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Stage")
	class UStageData* StageData = nullptr;


private:
	UPROPERTY()
	class UScoutingWidget* ScoutingWidget = nullptr;

	UPROPERTY()
	class UUserWidget* StageClearWidgetInstance = nullptr;
	
	UPROPERTY()
	APawn* SpawnedPlayerCharacter = nullptr;

	UPROPERTY()
	TArray<AActor*> SpawnedMeshActors;

	UPROPERTY()
	TArray<AActor*> SpawnedEnemyActors;
	
	UPROPERTY()
	TArray<AActor*> SpawnedGimmickActors;

	UPROPERTY()
	TArray<AActor*> SpawnedTriggerActors;

	UPROPERTY()
	AActor* SpawnedScoutCameraActor = nullptr;

	FTransform PlayerStartTransform;
	int32 CurrentStageIndex = 0;
	FTimerHandle StageResultTimerHandle;
};
