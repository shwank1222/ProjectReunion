#include "Core/OSMKGameState.h"

#include "OSMKSlowMotionSubsystem.h"
#include "Character/OSMKCharacterBase.h"
#include "Character/PlayerCharacter.h"
#include "Character/AI/EnemyCharacter.h"
#include "Character/ReplayCameraActor.h"
#include "GameMode/OSMKInGameGameMode.h"
#include "Kismet/GameplayStatics.h"

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
	// 다음 틱 전에 GameState 가 파괴되어도 안전하도록 WeakLambda 사용
	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		StartFailReplay();
	}));
}

void AOSMKGameState::StartClearReplay()
{
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

// ReSharper disable once CppMemberFunctionMayBeConst
void AOSMKGameState::StageClear()
{
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

// ReSharper disable once CppMemberFunctionMayBeConst
void AOSMKGameState::StageFailed()
{
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
	StageClear();
}

void AOSMKGameState::OnReplayFailedFinished()
{
	StageFailed();
}
