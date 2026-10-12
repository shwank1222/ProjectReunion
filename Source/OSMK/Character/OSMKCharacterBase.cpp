// Fill out your copyright notice in the Description page of Project Settings.


#include "OSMKCharacterBase.h"

#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/BodyInstance.h"

DEFINE_LOG_CATEGORY(LogCharacter);

AOSMKCharacterBase::AOSMKCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	PistolMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PistolMesh"));
	PistolMesh->SetupAttachment(GetMesh(), FName("HandGrip_R"));

	MuzzleEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MuzzleEffect"));
	MuzzleEffect->SetupAttachment(PistolMesh, MuzzleSocketName);
}

void AOSMKCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRecording)
	{
		RecordFrame();
	}
}

void AOSMKCharacterBase::ApplyDamage(const FVector& HitLocation, const FVector& ImpulseDirection)
{
	if (bIsDead)
	{
		return;
	}

	PendingHitLocation = HitLocation;
	PendingImpulseDirection = ImpulseDirection;

	Die();
}

void AOSMKCharacterBase::Die()
{
	if (bIsDead)
	{
		return;
	}

	OnCharacterDeath.Broadcast();
	GetCharacterMovement()->DisableMovement();
	bIsDead = true;
}

void AOSMKCharacterBase::StartRecording()
{
	RecordedFrames.Empty();
	RecordedFrameTimes.Empty();
	RecordingStartRealTime = 0.0;
	bIsRecording = true;
}

void AOSMKCharacterBase::StopRecording()
{
	bIsRecording = false;
}

void AOSMKCharacterBase::PreparePlayback()
{
	PlaybackCursor = 0;
	GetMesh()->WakeAllRigidBodies();
}

void AOSMKCharacterBase::PlaybackAtTime(float ElapsedSeconds)
{
	if (RecordedFrameTimes.IsEmpty())
	{
		return;
	}

	while (PlaybackCursor + 1 < RecordedFrameTimes.Num()
		&& RecordedFrameTimes[PlaybackCursor + 1] <= ElapsedSeconds)
	{
		++PlaybackCursor;
	}

	ApplyFrame(PlaybackCursor);
}

void AOSMKCharacterBase::ApplyFrame(int32 FrameIndex)
{
	if (!RecordedFrames.IsValidIndex(FrameIndex))
	{
		return;
	}

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!IsValid(MeshComp))
	{
		return;
	}

	const TArray<FTransform>& Frame = RecordedFrames[FrameIndex];
	const int32 BodyCount = FMath::Min(MeshComp->Bodies.Num(), Frame.Num());

	for (int32 i = 0; i < BodyCount; i++)
	{
		FBodyInstance* Body = MeshComp->Bodies[i];
		if (Body)
		{
			Body->SetBodyTransform(Frame[i], ETeleportType::TeleportPhysics);
			Body->SetLinearVelocity(FVector::ZeroVector, false);
			Body->SetAngularVelocityInRadians(FVector::ZeroVector, false);
		}
	}
}

void AOSMKCharacterBase::RecordFrame()
{
	const USkeletalMeshComponent* MeshComp = GetMesh();
	const UWorld* World = GetWorld();
	if (!IsValid(MeshComp) || !IsValid(World))
	{
		return;
	}

	if (RecordedFrames.IsEmpty())
	{
		RecordingStartRealTime = World->GetRealTimeSeconds();
	}

	TArray<FTransform> Frame;
	Frame.Reserve(MeshComp->Bodies.Num());

	for (FBodyInstance* Body : MeshComp->Bodies)
	{
		Frame.Add(Body ? Body->GetUnrealWorldTransform() : FTransform::Identity);
	}

	RecordedFrames.Add(MoveTemp(Frame));
	RecordedFrameTimes.Add(static_cast<float>(World->GetRealTimeSeconds() - RecordingStartRealTime));
}

void AOSMKCharacterBase::PlayFireMontage(const USkeletalMeshComponent* SkeletalMesh) const
{
	if (!IsValid(SkeletalMesh) || !IsValid(FireAnimMontage))
	{
		return;
	}

	if (UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance())
	{
		AnimInstance->Montage_Play(FireAnimMontage);
	}
}

void AOSMKCharacterBase::PlayFireSound() const
{
	if (!IsValid(FireSound))
	{
		UE_LOG(LogCharacter, Warning, TEXT("Fire Sound Is Invalid"));
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
}

void AOSMKCharacterBase::PlayFireEffect() const
{
	if (!IsValid(MuzzleEffect))
	{
		UE_LOG(LogCharacter, Warning, TEXT("Muzzle Effect Is Invalid"));
		return;
	}

	MuzzleEffect->Activate();
}
