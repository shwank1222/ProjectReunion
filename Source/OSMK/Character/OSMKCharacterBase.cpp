// Fill out your copyright notice in the Description page of Project Settings.


#include "OSMKCharacterBase.h"

AOSMKCharacterBase::AOSMKCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AOSMKCharacterBase::EnableRagdoll()
{
	// // Disable capsule collision
	// GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	
	GetMesh()->WakeAllRigidBodies();

	// Stop CharacterMovement
	// GetCharacterMovement()->DisableMovement();
}