// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "OSMKAIController.h"
#include "Character/PlayerCharacter.h"
#include "Core/OSMKGameState.h"

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

void AEnemyCharacter::Fire()
{
	FHitResult Hit;
	if (TrySweep(PlayerCharacter, Hit, 10000.0f))
	{
		UE_LOG(LogEnemy, Warning, TEXT("Hit: %s"), *Hit.GetActor()->GetName());

		if (APlayerCharacter* Player = Cast<APlayerCharacter>(Hit.GetActor()))
		{
			Player->ApplyDamage();
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

bool AEnemyCharacter::CanAttackTarget(AActor* TargetActor)
{
	if (!IsValid(TargetActor))
	{
		return false;
	}
	
	FHitResult Hit;
	const bool bHit = TrySweep(TargetActor, Hit, AttackRange);
	
	return bHit ? Hit.GetActor() == TargetActor : false;
}

void AEnemyCharacter::Die()
{
	Super::Die();
	
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetSimulatePhysics(true);
	
	GetMesh()->WakeAllRigidBodies();

	if (AOSMKGameState* GS = GetWorld()->GetGameState<AOSMKGameState>())
	{
		GS->NotifyEnemyKilled();
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::DestroyCharacter, 2.0f, false);
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
