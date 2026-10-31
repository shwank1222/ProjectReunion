// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/OSMKGameInstance.h"
#include "Core/OSMKGameState.h"
#include "Core/OSMKSaveGame.h"
#include "Core/OSMKSlowMotionSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/Bullets/BulletBase.h"


APlayerCharacter::APlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(FirstPersonCameraComponent);
	FirstPersonMesh->SetRelativeLocationAndRotation(FVector(-10.0f, 0.0f, -160.0f),
	                                                FRotator(0.0f, -90.0f, 0.0f));
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);
	
	PistolMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;
	PistolMesh->bOwnerNoSee = true;
	
	FirstPersonPistol = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP Pistol"));
	FirstPersonPistol->SetupAttachment(FirstPersonMesh, FName("HandGrip_R"));
	FirstPersonPistol->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonPistol->bOnlyOwnerSee = true;
	
	MuzzleEffect->SetupAttachment(FirstPersonPistol, MuzzleSocketName);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SlowMotionSubsystem = GetWorld()->GetSubsystem<UOSMKSlowMotionSubsystem>();

	if (AOSMKGameState* GS = GetWorld()->GetGameState<AOSMKGameState>())
	{
		OnCharacterDeath.AddUniqueDynamic(GS, &AOSMKGameState::PlayerDeath);
	}

	ResetAmmo();

	GetWorldTimerManager().SetTimer(WalkShakeTimerHandle, this, &ThisClass::UpdateWalkCameraShake, WalkShakeCheckInterval, true);
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameplayStatics::SetGlobalPitchModulation(this, 1.0f, 0.0f);

	Super::EndPlay(EndPlayReason);
}

void APlayerCharacter::UpdateWalkCameraShake()
{
	const APlayerController* PC = Cast<APlayerController>(GetController());
	const UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!WalkCameraShake || !PC || !PC->PlayerCameraManager || !Movement || !Movement->IsMovingOnGround())
	{
		StopWalkCameraShake();
		return;
	}

	const float Speed = GetVelocity().Size2D();
	if (Speed < WalkShakeMinSpeed)
	{
		StopWalkCameraShake();
		return;
	}

	if (!IsValid(ActiveWalkShake) || !ActiveWalkShake->IsActive())
	{
		ActiveWalkShake = PC->PlayerCameraManager->StartCameraShake(WalkCameraShake);
		WalkShakeCameraManager = PC->PlayerCameraManager;
	}

	if (IsValid(ActiveWalkShake))
	{
		const float MaxSpeed = Movement->GetMaxSpeed();
		const float SpeedRatio = MaxSpeed > KINDA_SMALL_NUMBER ? FMath::Clamp(Speed / MaxSpeed, 0.0f, 1.0f) : 1.0f;
		ActiveWalkShake->ShakeScale = WalkShakeScale * SpeedRatio;
	}
}

void APlayerCharacter::StopWalkCameraShake()
{
	if (IsValid(ActiveWalkShake) && WalkShakeCameraManager.IsValid())
	{
		WalkShakeCameraManager->StopCameraShake(ActiveWalkShake, false);
	}

	ActiveWalkShake = nullptr;
	WalkShakeCameraManager.Reset();
}

void APlayerCharacter::StartFireRecoil()
{
	if (!FirstPersonCameraComponent)
	{
		return;
	}

	RecoilPeak = FRotator(FireRecoilPitch, FMath::FRandRange(-FireRecoilYaw, FireRecoilYaw), 0.0f);
	RecoilElapsed = 0.0f;
	bRecoilActive = true;
	FirstPersonCameraComponent->bUsePawnControlRotation = false;

	GetWorldTimerManager().ClearTimer(RecoilTimerHandle);
	RecoilTimerHandle = GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::UpdateFireRecoil);
}

void APlayerCharacter::UpdateFireRecoil()
{
	if (!bRecoilActive || !FirstPersonCameraComponent)
	{
		return;
	}

	RecoilElapsed += GetWorld()->GetDeltaSeconds();

	float Alpha = 0.0f;
	if (RecoilElapsed < FireRecoilKickDuration)
	{
		Alpha = FMath::InterpEaseOut(0.0f, 1.0f, RecoilElapsed / FireRecoilKickDuration, 2.0f);
	}
	else if (RecoilElapsed < FireRecoilKickDuration + FireRecoilRecoverDuration)
	{
		Alpha = FMath::InterpEaseInOut(1.0f, 0.0f, (RecoilElapsed - FireRecoilKickDuration) / FireRecoilRecoverDuration, 2.0f);
	}
	else
	{
		StopFireRecoil();
		return;
	}

	FirstPersonCameraComponent->SetWorldRotation(GetControlRotation() + RecoilPeak * Alpha);

	RecoilTimerHandle = GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::UpdateFireRecoil);
}

void APlayerCharacter::StopFireRecoil()
{
	GetWorldTimerManager().ClearTimer(RecoilTimerHandle);
	bRecoilActive = false;
	RecoilElapsed = 0.0f;

	if (FirstPersonCameraComponent)
	{
		FirstPersonCameraComponent->bUsePawnControlRotation = true;
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::MoveInput);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::LookInput);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ThisClass::StartFiring);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ThisClass::OnHoldTriggered);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &ThisClass::CancelFiring);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ThisClass::CancelFiring);
	}
}

FVector APlayerCharacter::GetHeadWorldLocation() const
{
	const USkeletalMeshComponent* MeshComp = GetMesh();
	if (!IsValid(MeshComp))
	{
		return GetActorLocation() + FVector(0.0f, 0.0f, HeadFallbackHeight);
	}

	const FVector BoneLocation = MeshComp->GetBoneLocation(HeadSocketName);

	return BoneLocation.IsNearlyZero()
		       ? GetActorLocation() + FVector(0.0f, 0.0f, HeadFallbackHeight)
		       : BoneLocation;
}

void APlayerCharacter::Die()
{
	Super::Die();

	GetWorldTimerManager().ClearTimer(WalkShakeTimerHandle);
	StopWalkCameraShake();
	StopFireRecoil();

	DisableInput(Cast<APlayerController>(GetController()));

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();

	if (!PendingImpulseDirection.IsZero())
	{
		GetMesh()->AddImpulseAtLocation(PendingImpulseDirection * ImpulseStrength, PendingHitLocation);
	}
}

void APlayerCharacter::PrepareForReplay()
{
	GetMesh()->SetOwnerNoSee(false);
}

void APlayerCharacter::PrepareForVictoryShot()
{
	GetMesh()->SetOwnerNoSee(false);

	if (IsValid(PistolMesh))
	{
		PistolMesh->SetOwnerNoSee(false);
	}

	if (IsValid(FirstPersonMesh))
	{
		FirstPersonMesh->SetHiddenInGame(true);
	}

	if (IsValid(FirstPersonPistol))
	{
		FirstPersonPistol->SetHiddenInGame(true);
	}
}

void APlayerCharacter::LockForReplay()
{
	GetWorldTimerManager().ClearTimer(AutoFireTimerHandle);
	GetWorldTimerManager().ClearTimer(RestoreTimerHandle);

	StopHeartPulseSound();

	if (IsValid(SlowMotionSubsystem))
	{
		SlowMotionSubsystem->RestoreGimmickHighlight();
	}

	UGameplayStatics::SetGlobalPitchModulation(this, 1.0f, SlowFireSoundFadeTime);
	bSlowFireActive = false;

	GetWorldTimerManager().ClearTimer(WalkShakeTimerHandle);
	StopWalkCameraShake();
	StopFireRecoil();

	bIsFiring = false;
	bIsFired = false;

	DisableInput(Cast<APlayerController>(GetController()));
	GetCharacterMovement()->StopMovementImmediately();
}

void APlayerCharacter::MoveInput(const FInputActionValue& Value)
{
	if (!GetController())
	{
		return;
	}

	const FVector2D MovementVector = Value.Get<FVector2D>();

	AddMovementInput(GetActorRightVector(), MovementVector.X);
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
}

void APlayerCharacter::LookInput(const FInputActionValue& Value)
{
	if (!GetController())
	{
		return;
	}

	float Sensitivity = 1.0f;
	float SlowFireSensitivity = 0.5f;
	if (const UOSMKGameInstance* GI = GetGameInstance<UOSMKGameInstance>())
	{
		if (const UOSMKSaveGame* SaveGame = GI->GetSettingSaveGame())
		{
			Sensitivity = SaveGame->MouseSensitivity;
			SlowFireSensitivity = SaveGame->SlowFireSensitivity;
		}
	}

	const float LookScale = bSlowFireActive ? Sensitivity * SlowFireSensitivity * SlowFireMaxLookScale : Sensitivity;
	const FVector2D LookVector = Value.Get<FVector2D>() * LookScale;

	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void APlayerCharacter::StartFiring()
{
	if (LoadedAmmo.IsEmpty())
	{
		return;
	}

	if (bIsFired)
	{
		return;
	}

	bIsFiring = false;
	GetWorldTimerManager().SetTimer(AutoFireTimerHandle, this, &ThisClass::Fire, AutoFireDuration, false);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void APlayerCharacter::OnHoldTriggered()
{
	if (LoadedAmmo.IsEmpty())
	{
		return;
	}

	if (bIsFiring || bIsFired || !GetWorldTimerManager().IsTimerActive(AutoFireTimerHandle))
	{
		return;
	}

	if (IsValid(SlowMotionSubsystem))
	{
		SlowMotionSubsystem->ApplySlowMotion(0.2f, 10000.0f);
		SlowMotionSubsystem->ApplyGimmickHighlight();
		UGameplayStatics::SetGlobalPitchModulation(this, SlowFireSoundPitch, SlowFireSoundFadeTime);
		bSlowFireActive = true;
		
		PlayHeartPulseSound();
	}

	bIsFiring = true;
}

void APlayerCharacter::CancelFiring()
{
	if (!bIsFired && GetWorldTimerManager().IsTimerActive(AutoFireTimerHandle))
	{
		Fire();
	}
}

void APlayerCharacter::Fire()
{
	if (bIsFired)
	{
		return;
	}

	if (LoadedAmmo.IsEmpty())
	{
		UE_LOG(LogCharacter, Warning, TEXT("No Loaded Ammo"));
		return;
	}

	const TSoftClassPtr<ABulletBase> BulletClassSoft = LoadedAmmo[0].BulletBlueprint;

	PlayFireMontage(FirstPersonMesh);
	PlayFireSound();
	PlayFireEffect();
	StartFireRecoil();

	FireProjectile(BulletClassSoft.LoadSynchronous());

	bIsFired = true;
	bIsFiring = false;

	GetWorldTimerManager().ClearTimer(AutoFireTimerHandle);

	GetWorldTimerManager().SetTimer(RestoreTimerHandle, this, &ThisClass::StopSlowMotion, PostAutoFireDelay, false);

	UE_LOG(LogCharacter, Warning, TEXT("Fired!"));
}

void APlayerCharacter::StopSlowMotion()
{
	if (IsValid(SlowMotionSubsystem))
	{
		SlowMotionSubsystem->RestoreTimeDilation();
		SlowMotionSubsystem->RestoreGimmickHighlight();

		StopHeartPulseSound();
	}

	UGameplayStatics::SetGlobalPitchModulation(this, 1.0f, SlowFireSoundFadeTime);
	bSlowFireActive = false;

	bIsFired = false;
}

void APlayerCharacter::AddAmmo(const FName RowName)
{
	if (LoadedAmmo.Num() >= MaxAmmoCount)
	{
		UE_LOG(LogCharacter, Warning, TEXT("Loaded Ammo Is Full"));
		return;
	}

	const FBulletData* BulletData = GetBulletData(RowName);
	if (!BulletData)
	{
		UE_LOG(LogCharacter, Warning, TEXT("Invalid BulletData [RowName: %s]"), *RowName.ToString());
		return;
	}

	LoadedAmmo.Add(*BulletData);

	UE_LOG(LogCharacter, Warning, TEXT("[%s] Loaded"), *BulletData->BulletName.ToString());

	OnLoadedAmmoChanged.Broadcast();
}

void APlayerCharacter::RestoreAmmo()
{
	if (LoadedAmmo.IsEmpty())
	{
		UE_LOG(LogCharacter, Warning, TEXT("No Loaded Ammo"));
		return;
	}

	LoadedAmmo.RemoveAt(LoadedAmmo.Num() - 1);

	OnLoadedAmmoChanged.Broadcast();
}

void APlayerCharacter::ResetAmmo()
{
	LoadedAmmo.Empty();

	OnLoadedAmmoChanged.Broadcast();
}

void APlayerCharacter::FireProjectile(const TSubclassOf<ABulletBase> BulletClass)
{
	const FTransform ProjectileTransform = CalculateProjectileSpawnTransform(GetWeaponTargetLocation());

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetInstigator();
	
	GetWorld()->SpawnActor<ABulletBase>(BulletClass, ProjectileTransform, SpawnParams);

	LoadedAmmo.RemoveAt(0);

	OnLoadedAmmoChanged.Broadcast();
}

FVector APlayerCharacter::GetWeaponTargetLocation() const
{
	const FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	const FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * MaxAimDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredComponent(FirstPersonMesh.Get());

	FHitResult Hit;
	const bool bIsHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams);
	
	return bIsHit ? Hit.ImpactPoint : Hit.TraceEnd;
}

FTransform APlayerCharacter::CalculateProjectileSpawnTransform(const FVector& TargetLocation) const
{
	const FVector MuzzleLoc = FirstPersonPistol->GetSocketLocation(MuzzleSocketName);

	const FVector SpawnLoc = MuzzleLoc + ((TargetLocation - MuzzleLoc).GetSafeNormal() * MuzzleOffset);

	const FRotator AimRot = UKismetMathLibrary::FindLookAtRotation(SpawnLoc, TargetLocation);

	return FTransform(AimRot, SpawnLoc, FVector::OneVector * 0.05f);
}

void APlayerCharacter::PlayHeartPulseSound()
{
	if (!IsValid(HeartPulseSound))
	{
		UE_LOG(LogCharacter, Warning, TEXT("Invalid Heart Pulse Sound"));
		return;
	}

	if (!IsValid(HeartPulseSoundComponent))
	{
		UE_LOG(LogCharacter, Warning, TEXT("Create Heart Pulse Sound"));
		HeartPulseSoundComponent = UGameplayStatics::SpawnSoundAttached(HeartPulseSound, FirstPersonMesh);
	}
	else if (!HeartPulseSoundComponent->IsPlaying())
	{
		UE_LOG(LogCharacter, Warning, TEXT("Play Heart Pulse Sound"));
		HeartPulseSoundComponent->Play();
	}
}

void APlayerCharacter::StopHeartPulseSound() const
{
	if (IsValid(HeartPulseSoundComponent))
	{
		UE_LOG(LogCharacter, Warning, TEXT("Stop Heart Pulse Sound"));
		HeartPulseSoundComponent->Stop();
	}
}

FBulletData* APlayerCharacter::GetBulletData(const FName RowName) const
{
	if (!IsValid(BulletDataTable))
	{
		UE_LOG(LogCharacter, Warning, TEXT("Invalid bullet data"));
		return nullptr;
	}

	return BulletDataTable->FindRow<FBulletData>(RowName, TEXT("BulletData"));
}
