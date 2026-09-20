// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletBase.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class UProjectileMovementComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogBullet, Log, All);

UCLASS(Abstract)
class OSMK_API ABulletBase : public AActor
{
	GENERATED_BODY()

public:
	ABulletBase();

	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	                         const FHitResult& Hit);

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                            bool bFromSweep, const FHitResult& SweepResult);

	static void TriggerGimmick(AActor* OtherActor);

	void SpawnBulletHoleDecal(const FVector& Location, const FVector& ImpactNormal) const;

	void SpawnBulletHitEffect(const FVector& Location, const FVector& ImpactNormal) const;
	void SpawnBloodEffect(const FVector& Location, const FVector& ImpactNormal) const;

	void SpawnEffect(UNiagaraSystem* Effect, const FVector& Location, const FVector& ImpactNormal) const;

private:
	void EnemyAttack(AActor* OtherActor, const FHitResult& Hit) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UMeshComponent> MeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> TrailEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> BulletHitEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> BloodEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> BulletHoleDecal = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement = nullptr;

private:
	UPROPERTY(EditDefaultsOnly)
	float Lifespan = 3.0f;
};
