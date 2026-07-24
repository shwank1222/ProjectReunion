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
		CurrentStageState = EOSMKStageState::Clear;
		UE_LOG(LogTemp, Warning, TEXT("Stage Clear"));
		
		if (AOSMKInGameGameMode* GM = Cast<AOSMKInGameGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->HandleStageClear();
		}
		return;
	}

	if (DestroyedProjectileCount >= MaxBulletSlots)
	{
		CurrentStageState = EOSMKStageState::Failed;
		UE_LOG(LogTemp, Warning, TEXT("Stage Failed"));

		if (AOSMKInGameGameMode* GM = Cast<AOSMKInGameGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->HandleStageFail();
		}
		return;
	}
}