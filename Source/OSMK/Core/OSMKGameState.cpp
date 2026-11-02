#include "Core/OSMKGameState.h"

#include "OSMKSlowMotionSubsystem.h"
#include "OSMKBGMSubsystem.h"
#include "Character/OSMKCharacterBase.h"
#include "Character/PlayerCharacter.h"
#include "Character/AI/EnemyCharacter.h"
#include "Character/ReplayCameraActor.h"
#include "GameFramework/PlayerController.h"
#include "GameMode/OSMKInGameGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Ingame/OSMKIngameHUD.h"

DEFINE_LOG_CATEGORY_STATIC(LogOSMKGameState, Log, All);

void AOSMKGameState::BeginPlay()
{
	Super::BeginPlay();
}

void AOSMKGameState::EndScoutingPhase()
{
	CurrentStageState = EOSMKStageState::InProgress;

	if (UWorld* World = GetWorld())
	{
		if (UOSMKSlowMotionSubsystem* SlowMotion = World->GetSubsystem<UOSMKSlowMotionSubsystem>())
		{
			SlowMotion->RestoreGimmickHighlight();
		}
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UOSMKBGMSubsystem* BGM = GI->GetSubsystem<UOSMKBGMSubsystem>())
		{
			BGM->SetCombatLayer(true);
		}
	}
}

void AOSMKGameState::NotifyStageSpawned(int32 StageIndex, FName StageName)
{
	CurrentStageIndex = StageIndex;
	CurrentStageName = StageName;
	OnStageSpawned.Broadcast(StageIndex, StageName);
}

void AOSMKGameState::NotifyStageStarted()
{
	OnStageStarted.Broadcast(CurrentStageIndex, CurrentStageName);
}

void AOSMKGameState::RegisterEnemy(AEnemyCharacter* Enemy)
{
	if (!IsValid(Enemy) || Enemy->IsDead())
	{
		return;
	}

	bool bAlreadyRegistered = false;
	RegisteredEnemies.Add(Enemy, &bAlreadyRegistered);
	if (!bAlreadyRegistered)
	{
		TotalEnemyCount++;
		UpdateEnemyCount();
	}
}

void AOSMKGameState::UnregisterEnemy(AEnemyCharacter* Enemy)
{
	if (RegisteredEnemies.Remove(Enemy) > 0)
	{
		TotalEnemyCount = FMath::Max(0, TotalEnemyCount - 1);
		UpdateEnemyCount();
	}
}

void AOSMKGameState::ClearRegisteredEnemies()
{
	RegisteredEnemies.Empty();
	TotalEnemyCount = 0;
	UpdateEnemyCount();
}

void AOSMKGameState::ResetStageCounters()
{
	DestroyedProjectileCount = 0;
	UpdateEnemyCount();
}

void AOSMKGameState::UpdateEnemyCount()
{
	EnemyCount = RegisteredEnemies.Num();
	OnEnemyCountChanged.Broadcast();
}

void AOSMKGameState::NotifyEnemyKilled(AOSMKCharacterBase* KilledEnemy)
{
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(KilledEnemy);
	if (!Enemy || RegisteredEnemies.Remove(Enemy) == 0)
	{
		return;
	}

	UpdateEnemyCount();

	if (CurrentStageState != EOSMKStageState::InProgress)
	{
		return;
	}

	if (EnemyCount > 0)
	{
		if (IsValid(KilledEnemy))
		{
			KilledEnemy->FinalizeDeathAfterReplay();
		}
		return;
	}

	LastKilledTarget = KilledEnemy;
	StartClearReplay();
}

void AOSMKGameState::NotifyProjectileDestroyed()
{
	if (CurrentStageState != EOSMKStageState::InProgress)
	{
		return;
	}

	DestroyedProjectileCount++;

	if (DestroyedProjectileCount < MaxBulletSlots)
	{
		return;
	}

	TryOutOfAmmoFail();
}

void AOSMKGameState::NotifyGimmickTriggered()
{
	if (const UWorld* World = GetWorld())
	{
		LastGimmickTriggerTime = World->GetTimeSeconds();
	}
}

void AOSMKGameState::TryOutOfAmmoFail()
{
	if (CurrentStageState != EOSMKStageState::InProgress)
	{
		return;
	}

	if (LastGimmickTriggerTime >= 0.0f)
	{
		const float RemainingTime = LastGimmickTriggerTime + GimmickResolveDelay - GetWorld()->GetTimeSeconds();
		if (RemainingTime > 0.0f)
		{
			GetWorldTimerManager().SetTimer(OutOfAmmoTimerHandle, this, &ThisClass::TryOutOfAmmoFail, RemainingTime, false);
			return;
		}
	}

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!IsValid(Player))
	{
		return;
	}

	const FVector HeadLocation = Player->GetHeadWorldLocation();
	const AEnemyCharacter* NearestEnemy = FindNearestEnemy(HeadLocation);
	const FVector ImpulseDir = NearestEnemy
		? (HeadLocation - NearestEnemy->GetActorLocation()).GetSafeNormal()
		: FVector::ZeroVector;

	Player->ApplyDamage(HeadLocation, ImpulseDir);
}

AEnemyCharacter* AOSMKGameState::FindNearestEnemy(const FVector& FromLocation) const
{
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), Enemies);

	AEnemyCharacter* Nearest = nullptr;
	float MinDistSq = TNumericLimits<float>::Max();

	for (AActor* Enemy : Enemies)
	{
		AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(Enemy);
		if (!IsValid(EnemyCharacter))
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(EnemyCharacter->GetActorLocation(), FromLocation);
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			Nearest = EnemyCharacter;
		}
	}

	return Nearest;
}

void AOSMKGameState::ResetStageState()
{
	CurrentStageState = EOSMKStageState::Scouting;
	DestroyedProjectileCount = 0;
	LastGimmickTriggerTime = -1.0f;
	GetWorldTimerManager().ClearTimer(OutOfAmmoTimerHandle);
}

void AOSMKGameState::PlayerDeath()
{
	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		StartFailReplay();
	}));
}

void AOSMKGameState::StartClearReplay()
{
	if (!TryLockOutcome())
	{
		return;
	}

	SetIngameHUDVisible(false);
	LockPlayerForReplay();

	SpawnReplayCameraActor();

	if (!IsValid(SpawnedReplayCameraActor))
	{
		StageClear();
		return;
	}

	SpawnedReplayCameraActor->OnReplayFinished.AddUniqueDynamic(this, &ThisClass::OnReplayClearFinished);
	SpawnedReplayCameraActor->OnReplayFinalAngleStarted.AddUniqueDynamic(this, &ThisClass::OnReplayClearFinalAngleStarted);
	SpawnedReplayCameraActor->StartReplay(LastKilledTarget);
}

void AOSMKGameState::StartFailReplay()
{
	if (!TryLockOutcome())
	{
		return;
	}

	SetIngameHUDVisible(false);
	LockPlayerForReplay();

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	SpawnReplayCameraActor();

	if (!IsValid(SpawnedReplayCameraActor) || !IsValid(Player))
	{
		StageFailed();
		return;
	}

	SpawnedReplayCameraActor->OnReplayFinished.AddUniqueDynamic(this, &ThisClass::OnReplayFailedFinished);
	SpawnedReplayCameraActor->OnReplayFinalAngleStarted.AddUniqueDynamic(this, &ThisClass::OnReplayFailedFinalAngleStarted);
	SpawnedReplayCameraActor->StartReplay(Player);
}

void AOSMKGameState::SpawnReplayCameraActor()
{
	DestroyReplayCameraActor();

	if (!IsValid(ReplayCameraActorClass))
	{
		UE_LOG(LogOSMKGameState, Error, TEXT("ReplayCameraActorClass is not set"));
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnedReplayCameraActor = World->SpawnActor<AReplayCameraActor>(ReplayCameraActorClass, FTransform::Identity,
	                                                                SpawnParams);
}

void AOSMKGameState::DestroyReplayCameraActor()
{
	if (IsValid(SpawnedReplayCameraActor))
	{
		SpawnedReplayCameraActor->Destroy();
	}

	SpawnedReplayCameraActor = nullptr;
}

bool AOSMKGameState::TryLockOutcome()
{
	if (CurrentStageState != EOSMKStageState::InProgress)
	{
		return false;
	}

	CurrentStageState = EOSMKStageState::Recap;
	return true;
}

void AOSMKGameState::LockPlayerForReplay() const
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		Player->LockForReplay();
	}
}

void AOSMKGameState::SetIngameHUDVisible(bool bVisible) const
{
	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	const APlayerController* PC = World->GetFirstPlayerController();
	if (!IsValid(PC))
	{
		return;
	}

	if (AOSMKIngameHUD* HUD = Cast<AOSMKIngameHUD>(PC->GetHUD()))
	{
		HUD->SetHUDVisible(bVisible);
	}
}

void AOSMKGameState::StageClear()
{
	if (CurrentStageState != EOSMKStageState::Recap)
	{
		return;
	}

	CurrentStageState = EOSMKStageState::Clear;

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	if (UOSMKSlowMotionSubsystem* SlowMotion = World->GetSubsystem<UOSMKSlowMotionSubsystem>())
	{
		SlowMotion->RestoreTimeDilation();
		SlowMotion->RestoreGimmickHighlight();
	}

	if (AOSMKInGameGameMode* GM = Cast<AOSMKInGameGameMode>(World->GetAuthGameMode()))
	{
		GM->HandleStageClear();
	}
}

void AOSMKGameState::StageFailed()
{
	if (CurrentStageState != EOSMKStageState::Recap)
	{
		return;
	}

	CurrentStageState = EOSMKStageState::Failed;

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	if (UOSMKSlowMotionSubsystem* SlowMotion = World->GetSubsystem<UOSMKSlowMotionSubsystem>())
	{
		SlowMotion->RestoreTimeDilation();
		SlowMotion->RestoreGimmickHighlight();
	}

	if (AOSMKInGameGameMode* GM = Cast<AOSMKInGameGameMode>(World->GetAuthGameMode()))
	{
		GM->HandleStageFail();
	}
}

void AOSMKGameState::OnReplayClearFinished()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (IsValid(SpawnedReplayCameraActor) && IsValid(Player))
	{
		Player->PrepareForVictoryShot();
		SpawnedReplayCameraActor->BeginHoldShot(Player, true);
	}

	StageClear();
}

void AOSMKGameState::OnReplayFailedFinished()
{
	if (IsValid(SpawnedReplayCameraActor))
	{
		SpawnedReplayCameraActor->BeginHoldShot(nullptr, false);
	}

	StageFailed();
}

void AOSMKGameState::OnReplayClearFinalAngleStarted()
{
	StageClear();
}

void AOSMKGameState::OnReplayFailedFinalAngleStarted()
{
	StageFailed();
}
