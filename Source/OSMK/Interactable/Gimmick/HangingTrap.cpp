#include "Interactable/Gimmick/HangingTrap.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CableComponent.h"
#include "Character/OSMKCharacterBase.h"
#include "Kismet/GameplayStatics.h"

AHangingTrap::AHangingTrap()
{
	PrimaryActorTick.bCanEverTick = false;


	// Create root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);


	// Create trigger collision
	TriggerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerCollision"));
	TriggerCollision->SetupAttachment(Root);


	// Configure trigger overlap settings
	TriggerCollision->SetCollisionEnabled(
		ECollisionEnabled::QueryAndPhysics
	);

	TriggerCollision->SetCollisionResponseToAllChannels(
		ECR_Ignore
	);
	
	TriggerCollision->SetCollisionResponseToChannel(
		ECC_GameTraceChannel1, // Projectile channel
		ECR_Block
	);


	// Create rebar mesh components
	for(int32 Index = 0; Index < 6; Index++)
	{
		FString Name = FString::Printf(
			TEXT("Rebar_%02d"),
			Index
		);


		UStaticMeshComponent* Rebar =
			CreateDefaultSubobject<UStaticMeshComponent>(
				*Name
			);


		Rebar->SetupAttachment(Root);

		Rebar->SetCollisionProfileName(
			TEXT("PhysicsActor")
		);
		
		// Disable physics until the trap is triggered
		Rebar->SetSimulatePhysics(false);

		// Disable collision before activation
		Rebar->SetCollisionEnabled(
			ECollisionEnabled::NoCollision
		);

		// Enable hit event
		Rebar->SetNotifyRigidBodyCollision(true);

		Rebar->OnComponentHit.AddDynamic(
			this,
			&AHangingTrap::OnRebarHit
		);

		Rebars.Add(Rebar);
	}

	CableEndPoint = CreateDefaultSubobject<USceneComponent>(
		TEXT("CableEndPoint")
	);

	CableEndPoint->SetupAttachment(Root);
	
	// Create hanging cable
	Cable = CreateDefaultSubobject<UCableComponent>(
		TEXT("Cable")
	);

	Cable->SetupAttachment(Root);
	
	Cable->bAttachEnd = true;
	Cable->AttachEndTo.ComponentProperty = TEXT("CableEndPoint");

	// Configure cable appearance
	Cable->CableLength = 200.f;
	Cable->NumSegments = 10;
}

void AHangingTrap::BeginPlay()
{
	Super::BeginPlay();
}

void AHangingTrap::OnRebarHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (!HitComponent || !OtherActor)
	{
		return;
	}


	// Check ground collision
	if (OtherActor->ActorHasTag("Ground"))
	{
		UStaticMeshComponent* Rebar =
			Cast<UStaticMeshComponent>(HitComponent);


		if (Rebar)
		{
			// Play landing sound
			if (CollisionSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					CollisionSound,
					Hit.ImpactPoint
				);
			}


			// Disable hit event after landing
			Rebar->OnComponentHit.RemoveDynamic(
				this,
				&AHangingTrap::OnRebarHit
			);


			GIMMICK_LOG(Log, "Rebar landed.");
		}

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

void AHangingTrap::OnTriggered()
{
	GIMMICK_LOG(Log, "Triggered");
	
	for (UStaticMeshComponent* Rebar : Rebars)
	{
		if (!Rebar)
		{
			continue;
		}


		// Enable collision for falling object
		Rebar->SetCollisionEnabled(
			ECollisionEnabled::QueryAndPhysics
		);


		// Start falling
		Rebar->SetSimulatePhysics(true);
	}
}