#include "Interactable/Gimmick/PressurePipe.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "Components/ArrowComponent.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

#include "Character/OSMKCharacterBase.h"

APressurePipe::APressurePipe()
{
	PipeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PipeMesh"));
	PipeMesh->SetupAttachment(SceneRoot);
	
	BurstPoint = CreateDefaultSubobject<USceneComponent>(TEXT("BurstPoint"));
	BurstPoint->SetupAttachment(PipeMesh);
	
	BurstCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BurstCollision"));
	BurstCollision->SetupAttachment(BurstPoint);

	BurstArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("BurstArrow"));
	BurstArrow->SetupAttachment(BurstCollision);
	
	// Disable collision until burst starts.
	BurstCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BurstCollision->SetGenerateOverlapEvents(true);
}

void APressurePipe::BeginPlay()
{
	Super::BeginPlay();

	if (BurstCollision)
	{
		BurstCollision->OnComponentBeginOverlap.AddDynamic(
			this,
			&APressurePipe::OnBurstOverlap
		);
	}
}

void APressurePipe::OnBurstOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	AOSMKCharacterBase* Character =
		Cast<AOSMKCharacterBase>(OtherActor);

	if (!Character)
	{
		return;
	}

	HandleCharacterHit(Character);
}

void APressurePipe::OnTriggered()
{
	GIMMICK_LOG(Log, "Pressure Pipe Triggered.");

	// Start burst after delay.
	GetWorldTimerManager().SetTimer(
		BurstDelayTimerHandle,
		this,
		&APressurePipe::Burst,
		BurstDelay,
		false);
}

void APressurePipe::Burst()
{
	ActivateRelease();

	// Stop burst after duration.
	GetWorldTimerManager().SetTimer(
		BurstDurationTimerHandle,
		this,
		&APressurePipe::DeactivateRelease,
		BurstDuration,
		false);
}

void APressurePipe::ActivateRelease()
{
	// Enable overlap collision.
	BurstCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// Spawn burst effect.
	if (BurstEffect)
	{
		// Get burst direction from arrow component
		FRotator BurstRotation = BurstArrow->GetComponentRotation();

		// Apply arrow local rotation offset correction
		BurstRotation += BurstArrow->GetRelativeRotation();

		// Original Niagara local velocity range
		const FVector LocalMinVelocity(-10.f, -10.f, 700.f);
		const FVector LocalMaxVelocity(10.f, 10.f, 1000.f);

		// Convert local velocity range into world space
		const FVector WorldMinVelocity =
			BurstRotation.RotateVector(LocalMinVelocity);

		const FVector WorldMaxVelocity =
			BurstRotation.RotateVector(LocalMaxVelocity);


		EffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			BurstEffect,
			BurstPoint,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);


		if (EffectComponent)
		{
			// Pass velocity range to Niagara
			EffectComponent->SetNiagaraVariableVec3(
				TEXT("User.MinVelocity"),
				WorldMinVelocity
			);

			EffectComponent->SetNiagaraVariableVec3(
				TEXT("User.MaxVelocity"),
				WorldMaxVelocity
			);

			// Apply effect scale
			EffectComponent->SetRelativeScale3D(
				FVector(EffectScale)
			);
		}
	}

	// Play burst sound.
	if (BurstSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			BurstSound,
			BurstPoint->GetComponentLocation(),
			VolumeMultiplier);
	}
	
	GIMMICK_LOG(Log, "Pressure Pipe Release Activated.");
}

void APressurePipe::DeactivateRelease()
{
	// Disable overlap collision.
	BurstCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (EffectComponent)
	{
		EffectComponent->Deactivate();
		EffectComponent = nullptr;
	}
	GIMMICK_LOG(Log, "Pressure Pipe Release Deactivated.");
}
