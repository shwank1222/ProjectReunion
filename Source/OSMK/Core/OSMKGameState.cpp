#include "Core/OSMKGameState.h"

#include "GameMode/OSMKInGameGameMode.h"

void AOSMKGameState::EndScoutingPhase()
{
	CurrentStageState = EOSMKStageState::InProgress;
}

void AOSMKGameState::SetEnemyCount(int32 Count)
{
	EnemyCount = Count;
	DestroyedProjectileCount = 0;
	OnEnemyCountChanged.Broadcast();
}

void AOSMKGameState::NotifyEnemyKilled()
{
	if (CurrentStageState != EOSMKStageState::InProgress)
	{
		return;
	}

	EnemyCount = FMath::Max(0, EnemyCount - 1);
	OnEnemyCountChanged.Broadcast();
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

void AOSMKGameState::ResetStageState()
{
	CurrentStageState = EOSMKStageState::Scouting;
	EnemyCount = 0;
	DestroyedProjectileCount = 0;
}

void AOSMKGameState::CheckStageResult()
{
	if (EnemyCount <= 0)
	{
		if (AOSMKInGameGameMode* GM = Cast<AOSMKInGameGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->HandleStageClear();
		}
		return;
	}

	if (DestroyedProjectileCount >= MaxBulletSlots)
	{
		if (AOSMKInGameGameMode* GM = Cast<AOSMKInGameGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->HandleStageFail();
		}
		return;
	}
}