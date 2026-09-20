// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplayCameraActor.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "LevelInstance/LevelInstanceActor.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/OSMKCharacterBase.h"
#include "Core/OSMKSlowMotionSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogReplayCameraActor, Log, All);

AReplayCameraActor::AReplayCameraActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SetRootComponent(SpringArmComponent);
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->bInheritRoll = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
}

void AReplayCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsPlayingBack || !IsValid(TargetCharacter))
	{
		return;
	}

	if (CurrentPlaybackFrame >= TargetCharacter->GetRecordedFrameCount())
	{
		bIsPlayingBack = false;
		SetActorTickEnabled(false);
		OnAngleComplete();
		return;
	}

	TargetCharacter->PlaybackFrame(CurrentPlaybackFrame);
	CurrentPlaybackFrame++;
}

void AReplayCameraActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RestoreHiddenActors();

	Super::EndPlay(EndPlayReason);
}

void AReplayCameraActor::StartReplay(AOSMKCharacterBase* Target)
{
	if (!IsValid(Target))
	{
		UE_LOG(LogReplayCameraActor, Warning, TEXT("StartReplay: Target is invalid"));
		OnReplayFinished.Broadcast();
		Destroy();
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogReplayCameraActor, Warning, TEXT("StartReplay: World is invalid"));
		OnReplayFinished.Broadcast();
		Destroy();
		return;
	}

	if (UOSMKSlowMotionSubsystem* SlowMotion = World->GetSubsystem<UOSMKSlowMotionSubsystem>())
	{
		SlowMotion->ApplySlowMotion(ReplayTimeDilation, SlowMotionHoldDuration);
	}

	TargetCharacter = Target;
	CurrentAngleIndex = 0;

	TargetCharacter->PrepareForReplay();

	SetupAngle(0);
	TargetCharacter->StartRecording();

	GetWorldTimerManager().SetTimer(RecordingTimerHandle, this, &ThisClass::OnRecordingComplete, RecordDuration, false);
}

void AReplayCameraActor::HideActorsBetweenCameraAndTarget()
{
	if (!IsValid(TargetCharacter) || !IsValid(CameraComponent))
	{
		return;
	}

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(TargetCharacter);

	for (const FHitResult& Hit : GatherLineTraceActors(Params))
	{
		AActor* Actor = Hit.GetActor();
		if (!IsValid(Actor))
		{
			continue;
		}

		if (Actor->IsA<ALevelInstance>())
		{
			UPrimitiveComponent* Comp = Hit.GetComponent();
			if (IsValid(Comp) && Comp->IsVisible())
			{
				Comp->SetVisibility(false);
				HiddenComponents.AddUnique(Comp);
			}

			continue;
		}

		if (Actor->IsHidden())
		{
			continue;
		}

		Actor->SetActorHiddenInGame(true);
		HiddenActors.AddUnique(Actor);
	}

	for (AActor* Actor : GatherSphereOverlapActors(Params))
	{
		if (!IsValid(Actor) || Actor->IsHidden() || !ShouldHideActor(Actor))
		{
			continue;
		}

		Actor->SetActorHiddenInGame(true);
		HiddenActors.AddUnique(Actor);
	}
}

void AReplayCameraActor::RestoreHiddenActors()
{
	for (const TObjectPtr<AActor>& Actor : HiddenActors)
	{
		if (IsValid(Actor))
		{
			Actor->SetActorHiddenInGame(false);
		}
	}

	HiddenActors.Empty();

	for (const TObjectPtr<UPrimitiveComponent>& Comp : HiddenComponents)
	{
		if (IsValid(Comp))
		{
			Comp->SetVisibility(true);
		}
	}

	HiddenComponents.Empty();
}

void AReplayCameraActor::SetupAngle(int32 AngleIndex)
{
	if (!IsValid(TargetCharacter))
	{
		return;
	}

	const FVector PivotPos = TargetCharacter->GetActorLocation() + FVector(0.0f, 0.0f, CameraHeight);
	SetActorLocation(PivotPos);

	const FVector CameraPos = CalculateCameraPosition(AngleIndex);
	const FVector ArmDir = (CameraPos - PivotPos).GetSafeNormal();

	if (IsValid(SpringArmComponent))
	{
		SpringArmComponent->bDoCollisionTest = false;
		SpringArmComponent->TargetArmLength = CameraDistance;
		SpringArmComponent->SetWorldRotation(ArmDir.Rotation());
	}

	if (IsValid(HitEffect))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, TargetCharacter->GetLastHitLocation());
	}

	HideActorsBetweenCameraAndTarget();

	if (const UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			PC->SetViewTarget(this);
		}
	}
}

FVector AReplayCameraActor::CalculateCameraPosition(int32 AngleIndex) const
{
	if (!IsValid(TargetCharacter))
	{
		return GetActorLocation();
	}

	const FVector TargetLocation = TargetCharacter->GetActorLocation() + FVector(0.0f, 0.0f, CameraHeight);
	const FVector Forward = TargetCharacter->GetActorForwardVector();
	const FVector Right = TargetCharacter->GetActorRightVector();

	switch (AngleIndex)
	{
	case 0:
		return TargetLocation + Forward * CameraDistance;
	case 1:
		return TargetLocation + Right * CameraDistance;
	case 2:
		return TargetLocation - Forward * CameraDistance;
	default:
		return TargetLocation;
	}
}

TArray<FHitResult> AReplayCameraActor::GatherLineTraceActors(const FCollisionQueryParams& Params) const
{
	TArray<FHitResult> HitResults;

	const UWorld* World = GetWorld();
	if (!IsValid(World) || !IsValid(CameraComponent) || !IsValid(TargetCharacter))
	{
		return HitResults;
	}

	const FVector Start = CameraComponent->GetComponentLocation();
	const FVector End = TargetCharacter->GetActorLocation();
	World->LineTraceMultiByChannel(HitResults, Start, End, ECC_Visibility, Params);

	return HitResults;
}

TArray<AActor*> AReplayCameraActor::GatherSphereOverlapActors(const FCollisionQueryParams& Params) const
{
	TArray<AActor*> Result;

	const UWorld* World = GetWorld();
	if (!IsValid(World) || !IsValid(CameraComponent))
	{
		return Result;
	}

	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
		UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
		UEngineTypes::ConvertToObjectType(ECC_WorldDynamic)
	};
	const TArray<AActor*> IgnoredActors = {const_cast<AReplayCameraActor*>(this), TargetCharacter.Get()};

	const FVector SphereCenter = CameraComponent->GetComponentLocation();
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), SphereCenter, CameraProbeRadius, ObjectTypes, nullptr,
	                                          IgnoredActors, Result);

	return Result;
}

bool AReplayCameraActor::ShouldHideActor(const AActor* Actor) const
{
	if (!IsValid(Actor) || !IsValid(TargetCharacter) || !IsValid(CameraComponent))
	{
		return false;
	}

	const FVector ActorPos = Actor->GetActorLocation();
	const FVector TargetPos = TargetCharacter->GetActorLocation();
	const FVector CameraPos = CameraComponent->GetComponentLocation();

	if (ActorPos.Z < TargetPos.Z)
	{
		return false;
	}

	const FVector ToActor = ActorPos - TargetPos;
	const FVector CameraToTarget = TargetPos - CameraPos;

	return FVector::DotProduct(ToActor, CameraToTarget) <= 0.0f;
}

void AReplayCameraActor::OnRecordingComplete()
{
	if (IsValid(TargetCharacter))
	{
		TargetCharacter->StopRecording();
	}

	OnAngleComplete();
}

void AReplayCameraActor::OnAngleComplete()
{
	RestoreHiddenActors();

	CurrentAngleIndex++;

	if (CurrentAngleIndex >= ReplayAngleCount)
	{
		FinishReplay();
		return;
	}

	SetupAngle(CurrentAngleIndex);

	if (IsValid(TargetCharacter))
	{
		TargetCharacter->PreparePlayback();
	}

	CurrentPlaybackFrame = 0;
	bIsPlayingBack = true;
	SetActorTickEnabled(true);
}

void AReplayCameraActor::FinishReplay()
{
	GetWorldTimerManager().ClearTimer(RecordingTimerHandle);

	if (IsValid(TargetCharacter))
	{
		TargetCharacter->FinalizeDeathAfterReplay();
	}

	OnReplayFinished.Broadcast();
	Destroy();
}
