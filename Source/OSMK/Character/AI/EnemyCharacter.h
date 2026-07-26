// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/OSMKCharacterBase.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class OSMK_API AEnemyCharacter : public AOSMKCharacterBase
{
	GENERATED_BODY()

public:
	AEnemyCharacter();
	
	virtual void BeginPlay() override;
	
	void EquipPistol();
	
	void Fire();
	
	void ActivateEnemy() const;
	
	bool CanAttackTarget(AActor* TargetActor);
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsEquippedPistol() const { return bIsEquippedPistol; }
	
protected:
	virtual void Die() override;
	
	UPROPERTY(EditAnywhere)
	uint8 bAutoActivate : 1 = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Aim", meta = (AllowPrivateAccess = true))
	float AttackRange = 500.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimOffsetZ = 40.0f;

private:
	void DestroyCharacter();
	
	bool TrySweep(AActor* TargetActor, FHitResult& HitResult, float Distance);
	
	UPROPERTY()
	AActor* PlayerCharacter;
	
	uint8 bIsEquippedPistol : 1 = false;
};
