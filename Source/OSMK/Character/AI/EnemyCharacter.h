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

	virtual void FinalizeDeathAfterReplay() override;

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsEquippedPistol() const { return bIsEquippedPistol; }

protected:
	virtual void Die() override;

private:
	void DestroyCharacter();

	bool TrySweep(AActor* TargetActor, FHitResult& HitResult, float Distance);

protected:
	UPROPERTY(EditAnywhere)
	uint8 bAutoActivate : 1 = false;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AttackRange = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimOffsetZ = 40.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float MaxFireDistance = 10000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float DestroyDelay = 2.0f;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> IgnoreCollisionChannel = ECC_WorldStatic;

private:
	UPROPERTY()
	TObjectPtr<AActor> PlayerCharacter = nullptr;

	uint8 bIsEquippedPistol : 1 = false;

	FTimerHandle DestroyTimerHandle;
};
