// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletBase.generated.h"

class UNiagaraComponent;
class UProjectileMovementComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogBullet, Log, All);

UCLASS(Abstract)
class OSMK_API ABulletBase : public AActor
{
	GENERATED_BODY()

public:
	ABulletBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Destroyed() override;
	
protected:
	UFUNCTION()
	virtual void OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	                         const FHitResult& Hit);
	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                            bool bFromSweep, const FHitResult& SweepResult);

	static void TriggerGimmick(AActor* OtherActor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UMeshComponent> MeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> TrailEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

private:
	static void EnemyAttack(AActor* OtherActor);
	
	UPROPERTY(EditDefaultsOnly)
	float Lifespan = 3.0f;
};
