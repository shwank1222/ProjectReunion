// Fill out your copyright notice in the Description page of Project Settings.


#include "OSMKCharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogCharacter);

AOSMKCharacterBase::AOSMKCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AOSMKCharacterBase::ApplyDamage()
{
	if (bIsDead)
	{
		return;
	}
	
	Die();
}

void AOSMKCharacterBase::Die()
{
	if (bIsDead)
	{
		return;
	}
	
	OnCharacterDeath.Broadcast();
	
	EnableRagdoll();
	
	bIsDead = true;
}

void AOSMKCharacterBase::EnableRagdoll()
{
	// // Disable capsule collision
	// GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetSimulatePhysics(true);
	
	GetMesh()->WakeAllRigidBodies();

	// Stop CharacterMovement
	GetCharacterMovement()->DisableMovement();
}

void AOSMKCharacterBase::PlayFireSound() const
{
	if (!IsValid(FireSound))
	{
		UE_LOG(LogCharacter, Warning, TEXT("Invalid Fire Sound"));
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
}
