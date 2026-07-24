// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBase.h"

#include "Character/AI/EnemyCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interactable/Gimmick/GimmickBase.h"

DEFINE_LOG_CATEGORY(LogBullet);

ABulletBase::ABulletBase()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3500.0f;
	ProjectileMovement->ProjectileGravityScale = 0.2f;
	ProjectileMovement->bShouldBounce = false;
	
	InitialLifeSpan = Lifespan;
	
	MeshComponent->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnBulletHit);
	MeshComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBeginOverlap);
}

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABulletBase::OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	const FHitResult& Hit)
{
	UE_LOG(LogBullet, Warning, TEXT("[%s] Hit"), *GetName());
	
	if (IsValid(OtherActor))
	{
		UE_LOG(LogBullet, Warning, TEXT("Hit Actor: %s"), *OtherActor->GetName());
	}
	
	TriggerGimmick(OtherActor);
	EnemyAttack(OtherActor);
}

void ABulletBase::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogBullet, Warning, TEXT("[%s] Overlap"), *GetName());
	
	if (IsValid(OtherActor))
	{
		UE_LOG(LogBullet, Warning, TEXT("Overlap Actor: %s"), *OtherActor->GetName());
	}
	
	TriggerGimmick(OtherActor);
	EnemyAttack(OtherActor);
}

void ABulletBase::TriggerGimmick(AActor* OtherActor)
{
	if (AGimmickBase* Gimmick = Cast<AGimmickBase>(OtherActor))
	{
		Gimmick->Trigger();
	}
}

void ABulletBase::EnemyAttack(AActor* OtherActor)
{
	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor))
	{
		Enemy->ApplyDamage();
	}
}

