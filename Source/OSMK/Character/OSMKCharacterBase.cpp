// Fill out your copyright notice in the Description page of Project Settings.


#include "OSMKCharacterBase.h"

#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogCharacter);

AOSMKCharacterBase::AOSMKCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	PistolMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PistolMesh"));
	PistolMesh->SetupAttachment(GetMesh(), FName("HandGrip_R"));
	
	MuzzleEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MuzzleEffect"));
	MuzzleEffect->SetupAttachment(PistolMesh, MuzzleSocketName);
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
	
	GetCharacterMovement()->DisableMovement();
	
	bIsDead = true;
}

void AOSMKCharacterBase::PlayFireMontage(const USkeletalMeshComponent* SkeletalMesh) const
{
	if (IsValid(FireAnimMontage))
	{
		if (UAnimInstance* FirstPersonAnimInstance = SkeletalMesh->GetAnimInstance())
		{
			FirstPersonAnimInstance->Montage_Play(FireAnimMontage);
		}
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
