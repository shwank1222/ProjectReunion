// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/BulletData.h"
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

	FirstPersonPistol = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP Pistol"));
	FirstPersonPistol->SetupAttachment(FirstPersonMesh, FName("HandGrip_R"));
	FirstPersonPistol->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonPistol->bOnlyOwnerSee = true;

	ThirdPersonPistol = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP Pistol"));
	ThirdPersonPistol->SetupAttachment(GetMesh(), FName("HandGrip_R"));
	ThirdPersonPistol->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;
	ThirdPersonPistol->bOwnerNoSee = true;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	RemainingBulletNames = DefaultBulletNames;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::MoveInput);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::LookInput);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ThisClass::Fire);
	}
}

void APlayerCharacter::Die()
{
	Super::Die();
	
	DisableInput(Cast<APlayerController>(GetController()));
}

void APlayerCharacter::EnableRagdoll()
{
	Super::EnableRagdoll();
	
	FirstPersonMesh->SetCollisionProfileName(TEXT("Ragdoll"));
	FirstPersonMesh->SetSimulatePhysics(true);
	
	FirstPersonMesh->WakeAllRigidBodies();
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

void APlayerCharacter::Fire()
{
	if (RemainingBulletNames.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Remaining Bullets"));
		return;
	}
	
	FBulletData* BulletData = GetBulletData(RemainingBulletNames[0]);
	if (!BulletData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid bullet data"));
		return;
	}

	const TSoftClassPtr<ABulletBase> BulletClassSoft = BulletData->BulletBlueprint; 
	
	FireProjectile(BulletClassSoft.LoadSynchronous());
	
	UE_LOG(LogTemp, Warning, TEXT("Fired!"));

	PlayFireAnimation();
}

void APlayerCharacter::PlayFireAnimation() const
{
	if (IsValid(FireAnimMontage))
	{
		if (UAnimInstance* FirstPersonAnimInstance = FirstPersonMesh->GetAnimInstance())
		{
			FirstPersonAnimInstance->Montage_Play(FireAnimMontage);
		}
	}
}

void APlayerCharacter::FireProjectile(const TSubclassOf<ABulletBase> BulletClass)
{
	const FTransform ProjectileTransform = CalculateProjectileSpawnTransform(GetWeaponTargetLocation());
	
	// spawn the projectile
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetInstigator();

	GetWorld()->SpawnActor<ABulletBase>(BulletClass, ProjectileTransform, SpawnParams);

	// consume bullets
	RemainingBulletNames.RemoveAt(0);
}

FVector APlayerCharacter::GetWeaponTargetLocation() const
{
	const FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	const FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * MaxAimDistance);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredComponent(FirstPersonPistol.Get());
	
	FHitResult Hit;
	const bool bIsHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams);
	
	return bIsHit ? Hit.ImpactPoint : Hit.TraceEnd;
}

FTransform APlayerCharacter::CalculateProjectileSpawnTransform(const FVector& TargetLocation) const
{
	const FVector MuzzleLoc = FirstPersonPistol->GetSocketLocation(MuzzleSocketName);
	
	const FVector SpawnLoc = MuzzleLoc + ((TargetLocation - MuzzleLoc).GetSafeNormal() * MuzzleOffset);
	
	const FRotator AimRot = UKismetMathLibrary::FindLookAtRotation(SpawnLoc, TargetLocation + UKismetMathLibrary::RandomUnitVector());
	
	return FTransform(AimRot, SpawnLoc, FVector::OneVector * 0.1f);
}

FBulletData* APlayerCharacter::GetBulletData(const FName RowName) const
{
	if (!IsValid(BulletDataTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid bullet data"));
		return nullptr;
	}
	
	return BulletDataTable->FindRow<FBulletData>(RowName, TEXT("BulletData"));
}
