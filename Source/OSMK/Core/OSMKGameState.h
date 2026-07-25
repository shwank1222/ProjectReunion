#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "OSMKGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyCountChanged);

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
	void NotifyEnemyKilled();

	UFUNCTION(BlueprintCallable)
	void NotifyProjectileDestroyed();

	UFUNCTION(BlueprintCallable)
	void ResetStageState();

private:
	void CheckStageResult();
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnEnemyCountChanged OnEnemyCountChanged;

	UPROPERTY(BlueprintReadOnly)
	EOSMKStageState CurrentStageState = EOSMKStageState::Scouting;

	static constexpr int32 MaxBulletSlots = 6;

	UPROPERTY(BlueprintReadOnly)
	int32 EnemyCount = 0;

private:
	int32 DestroyedProjectileCount = 0;
};
