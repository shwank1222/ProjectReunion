#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "OSMKGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyCountChanged);

class AOSMKCharacterBase;
class AEnemyCharacter;
class AReplayCameraActor;

UENUM(BlueprintType)
enum class EOSMKStageState : uint8
{
	Scouting    UMETA(DisplayName = "Scouting"),
	InProgress  UMETA(DisplayName = "InProgress"),
	Clear       UMETA(DisplayName = "Clear"),
	Failed      UMETA(DisplayName = "Failed"),
	Recap       UMETA(DisplayName = "Recap")
};

UCLASS()
class OSMK_API AOSMKGameState : public AGameState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void EndScoutingPhase();

	UFUNCTION(BlueprintCallable)
	void SetEnemyCount(int32 Count);

	UFUNCTION(BlueprintCallable)
	void NotifyEnemyKilled(AOSMKCharacterBase* KilledEnemy);

	UFUNCTION(BlueprintCallable)
	void NotifyProjectileDestroyed();

	UFUNCTION(BlueprintCallable)
	void ResetStageState();

	UFUNCTION()
	void PlayerDeath();

	void DestroyReplayCameraActor();

protected:
	virtual void BeginPlay() override;

private:
	void StartClearReplay();
	void StartFailReplay();
	void SpawnReplayCameraActor();

	void SetIngameHUDVisible(bool bVisible) const;
	void CancelPlayerFireTimers() const;

	AEnemyCharacter* FindNearestEnemy(const FVector& FromLocation) const;

	UFUNCTION()
	void StageClear();

	UFUNCTION()
	void StageFailed();

	UFUNCTION()
	void OnReplayClearFinished();

	UFUNCTION()
	void OnReplayFailedFinished();

public:
	static constexpr int32 MaxBulletSlots = 6;

	UPROPERTY(BlueprintAssignable)
	FOnEnemyCountChanged OnEnemyCountChanged;

	UPROPERTY(BlueprintReadOnly)
	EOSMKStageState CurrentStageState = EOSMKStageState::Scouting;

	UPROPERTY(BlueprintReadOnly)
	int32 EnemyCount = 0;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Replay")
	TSubclassOf<AReplayCameraActor> ReplayCameraActorClass = nullptr;

private:
	UPROPERTY()
	TObjectPtr<AOSMKCharacterBase> LastKilledTarget = nullptr;

	UPROPERTY()
	TObjectPtr<AReplayCameraActor> SpawnedReplayCameraActor = nullptr;

	int32 DestroyedProjectileCount = 0;
};
