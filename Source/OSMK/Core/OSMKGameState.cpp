#include "Core/OSMKGameState.h"

#include "OSMKCutSceneManager.h"
#include "GameFramework/Character.h"
#include "GameMode/OSMKInGameGameMode.h"
#include "Kismet/GameplayStatics.h"

void AOSMKGameState::BeginPlay()
{
	Super::BeginPlay();
	
	CutSceneManager = UOSMKCutSceneManager::Get(this);
}

void AOSMKGameState::EndScoutingPhase()
{
	CurrentStageState = EOSMKStageState::InProgress;
}

void AOSMKGameState::SetEnemyCount(int32 Count)
{
	EnemyCount = Count;
	DestroyedProjectileCount = 0;
}

void AOSMKGameState::NotifyEnemyKilled(AActor* Enemy)
{
	if (CurrentStageState != EOSMKStageState::InProgress)
	{
		return;
	}
	
	LastDeadEnemy = Enemy;

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
		if (IsValid(CutSceneManager))
		{
			CutSceneManager->OnCutSceneFinished.AddUniqueDynamic(this, &ThisClass::StageClear);
			CutSceneManager->PlayCutScene(LastDeadEnemy, true);
		}
		return;
	}

	if (DestroyedProjectileCount >= MaxBulletSlots)
	{
		PlayerDeath();
		return;
	}
}

void AOSMKGameState::PlayerDeath()
{
	if (IsValid(CutSceneManager))
	{
		CutSceneManager->OnCutSceneFinished.AddUniqueDynamic(this, &ThisClass::StageFailed);
			
		ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		CutSceneManager->PlayCutScene(Player, false);
	}
}

void AOSMKGameState::StageClear()
{
	if (AOSMKInGameGameMode* GM = Cast<AOSMKInGameGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->HandleStageClear();
	}
	
	LastDeadEnemy = nullptr;
	
	UnbindCutSceneManagerDelegates();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AOSMKGameState::StageFailed()
{
	if (AOSMKInGameGameMode* GM = Cast<AOSMKInGameGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->HandleStageFail();
	}
	
	UnbindCutSceneManagerDelegates();
}

void AOSMKGameState::UnbindCutSceneManagerDelegates() const
{
	if (IsValid(CutSceneManager))
	{
		CutSceneManager->OnCutSceneFinished.RemoveAll(this);
	}
}
