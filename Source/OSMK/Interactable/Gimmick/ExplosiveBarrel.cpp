#include "Interactable/Gimmick/ExplosiveBarrel.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/OverlapResult.h"
#include "Character/OSMKCharacterBase.h"

#include "DrawDebugHelpers.h"

AExplosiveBarrel::AExplosiveBarrel()
{
	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelMesh"));
	SetRootComponent(BarrelMesh);
	
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	GeometryCollection->SetupAttachment(RootComponent);

	IgnitionPoint = CreateDefaultSubobject<USceneComponent>(TEXT("IgnitionPoint"));
	IgnitionPoint->SetupAttachment(BarrelMesh);
	
	// Hidden until explosion
	GeometryCollection->SetHiddenInGame(true);
	GeometryCollection->SetVisibility(false);
	GeometryCollection->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GeometryCollection->SetSimulatePhysics(false);
}

void AExplosiveBarrel::OnTriggered()
{
	GIMMICK_LOG(Log, "Triggered");
	// Spawn ignition effect
	
	if (IgnitionEffect)
	{
		IgnitionComponent =
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				IgnitionEffect,
				IgnitionPoint,
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				true);
		
		IgnitionComponent->SetRelativeScale3D(FVector(0.15f));
	}
	
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

	if (IgnitionComponent)
	{
		IgnitionComponent->Deactivate();
		IgnitionComponent = nullptr;
	}
	
	// Swap Static Mesh -> Geometry Collection
	BarrelMesh->SetHiddenInGame(true);
	BarrelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GeometryCollection->SetHiddenInGame(false);
	GeometryCollection->SetVisibility(true);
	GeometryCollection->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GeometryCollection->SetSimulatePhysics(true);


	// TODO : Apply explosion impulse to fractured pieces
	// Break Cluster
	GeometryCollection->ApplyExternalStrain(
		0,
		GetActorLocation(),
		ExplosionRadius,
		1,
		1.0f,
		ExplosionStrain
	);

	//Push Pieces
	GeometryCollection->AddRadialImpulse(
		GetActorLocation(),
		ExplosionRadius,
		ExplosionImpulse,
		ERadialImpulseFalloff::RIF_Linear,
		true
	);
	
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

	const FCollisionShape Sphere =
		FCollisionShape::MakeSphere(ExplosionRadius);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const bool bHit = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		Sphere,
		QueryParams);

#if WITH_EDITOR
	if (bShowExplosionDebug)
	{
		DrawDebugSphere(
			GetWorld(),
			GetActorLocation(),
			ExplosionRadius,
			32,
			FColor::Red,
			false,
			1.f,
			0,
			3.f);
	}
#endif

	if (!bHit)
	{
		GIMMICK_LOG(
			Log,
			TEXT("No actors detected in explosion radius"));

		return;
	}

	for (const FOverlapResult& Result : OverlapResults)
	{
		AOSMKCharacterBase* Character =
			Cast<AOSMKCharacterBase>(Result.GetActor());

		if (!Character)
		{
			continue;
		}
		HandleCharacterHit(Character);
		
		// Apply impulse to ragdoll
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			Mesh->AddRadialImpulse(
				GetActorLocation(),
				ExplosionRadius,
				ExplosionImpulse,
				ERadialImpulseFalloff::RIF_Linear,
				true                    // Velocity Change
			);
		}
	}
}


