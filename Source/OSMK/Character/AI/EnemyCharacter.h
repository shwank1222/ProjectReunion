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
	
	void Fire() const;
	
	void ActivateEnemy() const;
	
	bool CanAttackTarget(const AActor* TargetActor) const;
	
protected:
	virtual void Die() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> PistolMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> AttackArrow;
	
	UPROPERTY()
	TObjectPtr<ACharacter> PlayerCharacter;
	
	UPROPERTY(EditAnywhere)
	uint8 bAutoActivate : 1 = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	FName MuzzleName = FName("Muzzle");
	
	UPROPERTY(EditDefaultsOnly, Category = "Aim", meta = (AllowPrivateAccess = true))
	float AttackRange = 500.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	FRotator AimRotationRate = FRotator(0.0f, 180.0f, 0.0f);

private:
	void DestroyCharacter();
	
	bool TrySweep(FHitResult& HitResult, float Distance) const;
};
