#include "Core/OSMKGameState.h"

void AOSMKGameState::EndScoutingPhase()
{
	CurrentStageState = EOSMKStageState::InProgress;
}

void AOSMKGameState::SetEnemyCount(int32 Count)
{
	EnemyCount = Count;
	DestroyedProjectileCount = 0;
}

void AOSMKGameState::NotifyEnemyKilled()
{
	if (CurrentStageState != EOSMKStageState::InProgress)
	{
		return;
	}

	EnemyCount = FMath::Max(0, EnemyCount - 1);
	CheckStageResult();
}

void AOSMKGameState::NotifyProjectileDestroyed()
{
	if (CurrentStageState != EOSMKStageState::InProgress)
	{
		return;
	}

	DestroyedProjectileCount++;
	CheckStageResult();
}

void AOSMKGameState::CheckStageResult()
{
	if (EnemyCount <= 0)
	{
		CurrentStageState = EOSMKStageState::Clear;
		UE_LOG(LogTemp, Warning, TEXT("Stage Clear"));
		return;
	}

	if (DestroyedProjectileCount >= MaxBulletSlots)
	{
		CurrentStageState = EOSMKStageState::Failed;
		UE_LOG(LogTemp, Warning, TEXT("Stage Failed"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("EnemyCount: %d, DestroyedBulletCount: %d"), EnemyCount, DestroyedProjectileCount);
}
