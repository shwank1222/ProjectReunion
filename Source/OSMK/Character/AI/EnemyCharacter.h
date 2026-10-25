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

	void UnequipPistol();
	
	void Fire();
	
	void ActivateEnemy() const;

	bool CanAttackTarget(AActor* TargetActor, float ExtraRange = 0.0f);

	FORCEINLINE float GetAttackCancelMargin() const { return AttackCancelMargin; }

	bool UpdateAlert(AActor* TargetActor);

	FORCEINLINE bool IsAlerted() const { return bAlerted; }

	void ForceAlert();

	FORCEINLINE int32 GetAlertGroupId() const { return AlertGroupId; }
	FORCEINLINE void SetAlertGroupId(int32 InGroupId) { AlertGroupId = InGroupId; }

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

	UPROPERTY(EditDefaultsOnly, Category = "Aim", meta = (ClampMin = "0.0"))
	float AttackCancelMargin = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimOffsetZ = 40.0f;

	UPROPERTY(EditAnywhere, Category = "Alert", meta = (ClampMin = "0.0"))
	float AlertPathDistance = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Alert", meta = (ClampMin = "0.0"))
	float AlertCheckInterval = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float DestroyDelay = 2.0f;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> IgnoreCollisionChannel = ECC_WorldStatic;

private:
	UPROPERTY()
	TObjectPtr<AActor> PlayerCharacter = nullptr;

	uint8 bIsEquippedPistol : 1 = false;
	uint8 bAlerted : 1 = false;

	double LastAlertCheckTime = -1.0;
	int32 AlertGroupId = INDEX_NONE;

	void AlertGroup() const;

	FTimerHandle DestroyTimerHandle;
};
