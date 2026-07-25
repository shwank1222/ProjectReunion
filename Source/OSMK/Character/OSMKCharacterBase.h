// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OSMKCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeath);

DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All);

UCLASS()
class OSMK_API AOSMKCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AOSMKCharacterBase();
	
	void ApplyDamage();
	
	UPROPERTY(BlueprintAssignable)
	FOnCharacterDeath OnCharacterDeath;
	
protected:
	virtual void Die();
	virtual void EnableRagdoll();
	
	void PlayFireSound() const;
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	uint8 bIsDead : 1 = false;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> FireSound;
};
