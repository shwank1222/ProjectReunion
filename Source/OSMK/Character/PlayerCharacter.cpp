// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/OSMKSlowMotionSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/Bullets/BulletBase.h"


APlayerCharacter::APlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f),
	                                                           FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

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
	
	ResetAmmo();
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

void APlayerCharacter::Die()
{
	Super::Die();

	DisableInput(Cast<APlayerController>(GetController()));
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

	const FVector2D LookVector = Value.Get<FVector2D>();

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

	if (bIsFiring)
	{
		return;
	}
	
	if (IsValid(SlowMotionSubsystem))
	{
		SlowMotionSubsystem->ApplySlowMotion(0.2f, 10000.0f);
		SlowMotionSubsystem->ApplyGimmickHighlight();
		
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
		UE_LOG(LogTemp, Warning, TEXT("No Loaded Ammo"));
		return;
	}

	const TSoftClassPtr<ABulletBase> BulletClassSoft = LoadedAmmo[0].BulletBlueprint;

	PlayFireMontage(FirstPersonMesh);
	PlayFireSound();
	PlayFireEffect();

	FireProjectile(BulletClassSoft.LoadSynchronous());

	bIsFired = true;
	bIsFiring = false;

	GetWorldTimerManager().ClearTimer(AutoFireTimerHandle);

	GetWorldTimerManager().SetTimer(RestoreTimerHandle, this, &ThisClass::StopSlowMotion, PostAutoFireDelay, false);

	UE_LOG(LogTemp, Warning, TEXT("Fired!"));
}

void APlayerCharacter::StopSlowMotion()
{
	if (IsValid(SlowMotionSubsystem))
	{
		SlowMotionSubsystem->RestoreTimeDilation();
		SlowMotionSubsystem->RestoreGimmickHighlight();
	
		StopHeartPulseSound();
	}

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

	return FTransform(AimRot, SpawnLoc, FVector::OneVector * 0.1f);
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
