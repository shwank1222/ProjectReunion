// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "OSMKAIController.h"
#include "Character/PlayerCharacter.h"
#include "Components/ArrowComponent.h"
#include "Core/OSMKGameState.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogEnemy, Log, All);

AEnemyCharacter::AEnemyCharacter()
{
	PistolMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PistolMesh"));
	PistolMesh->SetupAttachment(GetMesh(), FName("HandGrip_R"));

	AttackArrow = CreateDefaultSubobject<UArrowComponent>(FName("AttackArrow"));
	AttackArrow->SetupAttachment(RootComponent);
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	if (bAutoActivate)
	{
		ActivateEnemy();
	}
}

void AEnemyCharacter::Fire() const
{
	FHitResult Hit;
	if (TrySweep(Hit, 10000.0f))
	{
		UE_LOG(LogEnemy, Warning, TEXT("Hit: %s"), *Hit.GetActor()->GetName());

		if (APlayerCharacter* Player = Cast<APlayerCharacter>(Hit.GetActor()))
		{
			Player->ApplyDamage();
		}
	}
	
	PlayFireSound();
}

void AEnemyCharacter::ActivateEnemy() const
{
	if (AOSMKAIController* AIController = Cast<AOSMKAIController>(GetController()))
	{
		AIController->ActivateLogic();
	}
}

bool AEnemyCharacter::CanAttackTarget(const AActor* TargetActor) const
{
	if (!IsValid(TargetActor))
	{
		return false;
	}
	
	FHitResult Hit;
	const bool bHit = TrySweep(Hit, AttackRange);
	
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

	SetActorHiddenInGame(true);
	
	// Destroy();
}

bool AEnemyCharacter::TrySweep(FHitResult& HitResult, const float Distance) const
{
	const FVector Start = AttackArrow->GetComponentLocation();
	const FVector Direction = AttackArrow->GetForwardVector();
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
