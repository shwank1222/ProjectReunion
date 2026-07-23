#include "Interactable/Gimmick/ExplosiveBarrel.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"

AExplosiveBarrel::AExplosiveBarrel()
{
	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelMesh"));
	SetRootComponent(BarrelMesh);
	
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	GeometryCollection->SetupAttachment(RootComponent);


	// Hidden until explosion
	GeometryCollection->SetHiddenInGame(true);
	GeometryCollection->SetVisibility(false);
	GeometryCollection->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GeometryCollection->SetSimulatePhysics(false);
}

void AExplosiveBarrel::OnTriggered()
{
	GIMMICK_LOG(Log, "Triggered");
	
	GetWorldTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&AExplosiveBarrel::Explode,
		ExplosionDelay,
		false);
}

void AExplosiveBarrel::Explode()
{
	GIMMICK_LOG(Log, "Exploded");

	// Swap Static Mesh -> Geometry Collection
	BarrelMesh->SetHiddenInGame(true);
	BarrelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GeometryCollection->SetHiddenInGame(false);
	GeometryCollection->SetVisibility(true);
	GeometryCollection->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GeometryCollection->SetSimulatePhysics(true);


	// TODO : Apply explosion impulse to fractured pieces


	// Spawn Niagara explosion effect
	if (ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionEffect,
			GetActorLocation()
		);
	}


	// Play explosion sound
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ExplosionSound,
			GetActorLocation()
		);
	}


	// Kill enemies in explosion radius
	ScanExplosionRadius();


	// TODO : Trigger nearby gimmicks (Chain Reaction)
	// TODO : Destroy actor after a few seconds
}

void AExplosiveBarrel::ScanExplosionRadius()
{
	TArray<FOverlapResult> OverlapResults;

	FCollisionShape Sphere =
		FCollisionShape::MakeSphere(ExplosionRadius);


	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);


	bool bHit = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		Sphere,
		QueryParams
	);


	if (bShowExplosionDebug)
	{
		DrawDebugSphere(
			GetWorld(),
			GetActorLocation(),
			ExplosionRadius,
			32,
			FColor::Red,
			false,
			3.f,
			0,
			3.f
		);
	}


	GIMMICK_LOG(
		Log,
		TEXT("Explosion scan started. Radius: %.1f"),
		ExplosionRadius
	);


	if (!bHit)
	{
		GIMMICK_LOG(
			Log,
			TEXT("No actors detected in explosion radius")
		);

		return;
	}
}
