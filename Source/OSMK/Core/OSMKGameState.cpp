#include "Core/OSMKGameState.h"

#include "OSMKCutsceneManager.h"
#include "Character/PlayerCharacter.h"
#include "GameFramework/Character.h"
#include "GameMode/OSMKInGameGameMode.h"
#include "Kismet/GameplayStatics.h"

void AOSMKGameState::BeginPlay()
{
	Super::BeginPlay();
	
	CutsceneManager = UOSMKCutsceneManager::Get(this);
}

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
		if (IsValid(CutsceneManager))
		{
			CutsceneManager->OnCutsceneFinished.AddUniqueDynamic(this, &ThisClass::StageClear);
			CutsceneManager->PlayCutscene(true);
		}
		return;
	}

	if (DestroyedProjectileCount >= MaxBulletSlots)
	{
		if (APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			Player->ApplyDamage();
		}
		return;
	}
}

void AOSMKGameState::PlayerDeath()
{
	if (IsValid(CutsceneManager))
	{
		CutsceneManager->OnCutsceneFinished.AddUniqueDynamic(this, &ThisClass::StageFailed);
		
		CutsceneManager->PlayCutscene(false);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AOSMKGameState::StageClear()
{
	if (AOSMKInGameGameMode* GM = Cast<AOSMKInGameGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->HandleStageClear();
	}
	
	UnbindCutsceneManagerDelegates();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AOSMKGameState::StageFailed()
{
	if (AOSMKInGameGameMode* GM = Cast<AOSMKInGameGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->HandleStageFail();
	}
	
	UnbindCutsceneManagerDelegates();
}

void AOSMKGameState::UnbindCutsceneManagerDelegates() const
{
	if (IsValid(CutsceneManager))
	{
		CutsceneManager->OnCutsceneFinished.RemoveAll(this);
	}
}
