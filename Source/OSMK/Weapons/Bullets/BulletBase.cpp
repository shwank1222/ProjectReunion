// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBase.h"

#include "GameFramework/ProjectileMovementComponent.h"

ABulletBase::ABulletBase()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3500.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	
	InitialLifeSpan = Lifespan;
}

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();
	
}

