// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"


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

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::MoveInput);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::LookInput);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ThisClass::Fire);
	}
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
	UE_LOG(LogTemp, Warning, TEXT("Fire!"));

	if (IsValid(FireAnimMontage))
	{
		if (UAnimInstance* FirstPersonAnimInstance = FirstPersonMesh->GetAnimInstance())
		{
			FirstPersonAnimInstance->Montage_Play(FireAnimMontage);
		}
	}
}
