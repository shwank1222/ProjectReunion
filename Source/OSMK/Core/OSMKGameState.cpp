#include "Core/OSMKGameState.h"

#include "OSMKSlowMotionSubsystem.h"
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
}

void AOSMKGameState::SetEnemyCount(int32 Count)
{
	EnemyCount = Count;
	DestroyedProjectileCount = 0;
	OnEnemyCountChanged.Broadcast();
}

void AOSMKGameState::NotifyEnemyKilled(AOSMKCharacterBase* KilledEnemy)
{
	if (CurrentStageState != EOSMKStageState::InProgress)
	{
		return;
	}

	EnemyCount = FMath::Max(0, EnemyCount - 1);
	OnEnemyCountChanged.Broadcast();

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
	EnemyCount = 0;
	DestroyedProjectileCount = 0;
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
