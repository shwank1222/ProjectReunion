#pragma once

#include "CoreMinimal.h"
#include "Data/StageData.h"
#include "Engine/LevelStreamingDynamic.h"
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
	void ClearStage();

	UFUNCTION()
	void OnLevelInstanceLoaded();

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
	void AdvanceRules();
	void OnAllRulesFinished();
	void SetupScoutCamera(AActor* CameraActor);
	void ShowStageClearWidget();
	void ShowStageFailWidget();
	void ShowCredits();
	void ProceedToNextStage();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UScoutingWidget> ScoutingWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> StageClearWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> StageFailWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UCreditsWidget> CreditsWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Stage")
	FName TitleLevelName = TEXT("L_Title");

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
	TArray<TObjectPtr<AActor>> SpawnedActors;

	UPROPERTY()
	TArray<TObjectPtr<ULevelStreamingDynamic>> SpawnedLevelStreamings;

	UPROPERTY()
	AActor* SpawnedScoutCameraActor = nullptr;

	UPROPERTY()
	class UStageExtractedCache* LoadedCache = nullptr;

	FTransform PlayerStartTransform;
	FName CurrentLevelName = NAME_None;
	int32 CurrentStageIndex = 0;
	int32 CurrentRuleIndex = 0;
	int32 PendingAsyncSpawns = 0;
	bool bAdvancingRules = false;
	FTimerHandle StageResultTimerHandle;
};
