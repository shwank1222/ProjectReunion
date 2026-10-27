// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "OSMKAIController.h"
#include "Character/PlayerCharacter.h"
#include "Core/OSMKGameState.h"
#include "GameMode/OSMKInGameGameMode.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogEnemy, Log, All);

AEnemyCharacter::AEnemyCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	PistolMesh->SetHiddenInGame(true);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (bAutoActivate)
	{
		ActivateEnemy();
	}
}

void AEnemyCharacter::EquipPistol()
{
	if (bIsEquippedPistol)
	{
		return;
	}
	
	PistolMesh->SetHiddenInGame(false);
	bIsEquippedPistol = true;
}

void AEnemyCharacter::UnequipPistol()
{
	if (!bIsEquippedPistol)
	{
		return;
	}

	PistolMesh->SetHiddenInGame(true);
	bIsEquippedPistol = false;
}

void AEnemyCharacter::Fire()
{
	FHitResult Hit;
	if (TrySweep(PlayerCharacter, Hit, AttackRange + AttackCancelMargin))
	{
		UE_LOG(LogEnemy, Warning, TEXT("Hit: %s"), *GetNameSafe(Hit.GetActor()));

		if (APlayerCharacter* Player = Cast<APlayerCharacter>(Hit.GetActor()))
		{
			const FVector MuzzleLocation = IsValid(PistolMesh)
				                               ? PistolMesh->GetSocketLocation(MuzzleSocketName)
				                               : GetActorLocation();
			const FVector ImpulseDirection = (Hit.ImpactPoint - MuzzleLocation).GetSafeNormal();

			Player->ApplyDamage(Hit.ImpactPoint, ImpulseDirection);
		}
	}
	
	PlayFireMontage(GetMesh());
	PlayFireSound();
	PlayFireEffect();
}

void AEnemyCharacter::ActivateEnemy() const
{
	if (AOSMKAIController* AIController = Cast<AOSMKAIController>(GetController()))
	{
		AIController->ActivateLogic();
	}
}

bool AEnemyCharacter::UpdateAlert(AActor* TargetActor)
{
	if (bAlerted || !IsValid(TargetActor))
	{
		return bAlerted;
	}

	const double Now = GetWorld()->GetTimeSeconds();
	if (LastAlertCheckTime >= 0.0 && Now - LastAlertCheckTime < AlertCheckInterval)
	{
		return false;
	}
	LastAlertCheckTime = Now;

	const UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(this, GetActorLocation(), TargetActor->GetActorLocation(), this);
	if (!Path || !Path->IsValid() || Path->IsPartial())
	{
		return false;
	}

	if (Path->GetPathLength() <= AlertPathDistance)
	{
		bAlerted = true;
		AlertGroup();
	}

	return bAlerted;
}

void AEnemyCharacter::ForceAlert()
{
	if (bIsDead)
	{
		return;
	}

	bAlerted = true;
}

void AEnemyCharacter::AlertGroup() const
{
	if (AOSMKInGameGameMode* GM = Cast<AOSMKInGameGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->AlertEnemyGroup(AlertGroupId);
	}
}

bool AEnemyCharacter::CanAttackTarget(AActor* TargetActor, const float ExtraRange)
{
	if (!IsValid(TargetActor))
	{
		return false;
	}

	FHitResult Hit;
	const bool bHit = TrySweep(TargetActor, Hit, AttackRange + ExtraRange);
	
	return bHit ? Hit.GetActor() == TargetActor : false;
}

void AEnemyCharacter::Die()
{
	Super::Die();

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(IgnoreCollisionChannel, ECR_Ignore);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();

	if (!PendingImpulseDirection.IsZero())
	{
		GetMesh()->AddImpulseAtLocation(PendingImpulseDirection * ImpulseStrength, PendingHitLocation);
	}

	AlertGroup();

	if (AOSMKGameState* GS = GetWorld()->GetGameState<AOSMKGameState>())
	{
		GS->NotifyEnemyKilled(this);
	}
}

void AEnemyCharacter::FinalizeDeathAfterReplay()
{
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &ThisClass::DestroyCharacter, DestroyDelay, false);
}

void AEnemyCharacter::DestroyCharacter()
{
	if (const AOSMKAIController* AIController = Cast<AOSMKAIController>(GetController()))
	{
		AIController->DeactivateLogic(TEXT("Enemy Dead"));
	}
	
	Destroy();
}

bool AEnemyCharacter::TrySweep(AActor* TargetActor, FHitResult& HitResult, const float Distance)
{
	PlayerCharacter = TargetActor;
	if (!IsValid(PlayerCharacter))
	{
		return false;
	}
	
	const FVector Start = PistolMesh->GetSocketLocation(MuzzleSocketName);
	FVector TargetLocation = PlayerCharacter->GetActorLocation();
	TargetLocation.Z += AimOffsetZ;
	const FVector Direction = (TargetLocation - Start).GetSafeNormal();
	const FVector End = Start + Direction * Distance;

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.0f);

	float ShapeRadius = 10.0f;
	
	FCollisionShape Shape;
	Shape.SetSphere(ShapeRadius);
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredComponent(PistolMesh.Get());
	
	const bool bHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, ECC_Visibility, Shape, Params);
	
	const FVector HitLocation = bHit ? HitResult.Location : End;
	
	DrawDebugSphere(GetWorld(), HitLocation, ShapeRadius, 12, FColor::Green, false, 0.0f);
	
	return bHit;
}
