// Fill out your copyright notice in the Description page of Project Settings.


#include "RicochetBullet.h"

#include "Character/AI/EnemyCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ARicochetBullet::ARicochetBullet()
{
	ProjectileMovement->bShouldBounce = true;
}

void ARicochetBullet::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->OnProjectileBounce.AddUniqueDynamic(this, &ThisClass::OnProjectileBounce);
}

void ARicochetBullet::OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                  const FHitResult& Hit)
{
	Super::OnBulletHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

	RegisterEnemyHit(OtherActor);
}

void ARicochetBullet::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                     bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	RegisterEnemyHit(OtherActor);
}

void ARicochetBullet::RegisterEnemyHit(AActor* OtherActor)
{
	if (!Cast<AEnemyCharacter>(OtherActor))
	{
		return;
	}

	HitEnemies.Add(OtherActor);

	if (HitEnemies.Num() >= MaxEnemyHitCount)
	{
		Destroy();
	}
}

void ARicochetBullet::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	const float Speed = ProjectileMovement->Velocity.Size();
	if (Speed <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	const FVector Origin = GetActorLocation();
	const FVector BounceDirection = ClampUpwardDirection(ProjectileMovement->Velocity / Speed, ImpactVelocity);

	FVector NewDirection = BounceDirection;
	if (const AEnemyCharacter* Target = FindRicochetTarget(Origin, BounceDirection, ImpactResult.GetActor()))
	{
		const FVector TargetDirection = (Target->GetActorLocation() - Origin).GetSafeNormal();
		NewDirection = FMath::Lerp(BounceDirection, TargetDirection, AimAssistStrength).GetSafeNormal();
	}

	ProjectileMovement->Velocity = NewDirection * Speed;
	SetActorRotation(NewDirection.Rotation());
}

FVector ARicochetBullet::ClampUpwardDirection(const FVector& Direction, const FVector& ImpactVelocity) const
{
	const float MaxZ = FMath::Sin(FMath::DegreesToRadians(MaxUpwardAngle));
	if (Direction.Z <= MaxZ)
	{
		return Direction;
	}

	FVector Horizontal = FVector(Direction.X, Direction.Y, 0.0f).GetSafeNormal();
	if (Horizontal.IsNearlyZero())
	{
		Horizontal = FVector(ImpactVelocity.X, ImpactVelocity.Y, 0.0f).GetSafeNormal();
	}
	if (Horizontal.IsNearlyZero())
	{
		return Direction;
	}

	return Horizontal * FMath::Cos(FMath::DegreesToRadians(MaxUpwardAngle)) + FVector::UpVector * MaxZ;
}

AEnemyCharacter* ARicochetBullet::FindRicochetTarget(const FVector& Origin, const FVector& BounceDirection, const AActor* IgnoreActor) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(World, AEnemyCharacter::StaticClass(), Enemies);

	const float MinDot = FMath::Cos(FMath::DegreesToRadians(AimAssistAngle));
	const float MaxDistSq = FMath::Square(AimAssistRadius);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(RicochetTarget), false, this);

	AEnemyCharacter* BestTarget = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (AActor* Actor : Enemies)
	{
		AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Actor);
		if (!IsValid(Enemy) || Enemy == IgnoreActor || Enemy->IsDead())
		{
			continue;
		}

		const FVector ToEnemy = Enemy->GetActorLocation() - Origin;
		const float DistSq = ToEnemy.SizeSquared();
		if (DistSq > MaxDistSq || DistSq >= BestDistSq)
		{
			continue;
		}

		if (FVector::DotProduct(ToEnemy.GetSafeNormal(), BounceDirection) < MinDot)
		{
			continue;
		}

		FHitResult Hit;
		if (World->LineTraceSingleByChannel(Hit, Origin, Enemy->GetActorLocation(), ECC_Visibility, QueryParams) && Hit.GetActor() != Enemy)
		{
			continue;
		}

		BestTarget = Enemy;
		BestDistSq = DistSq;
	}

	return BestTarget;
}
