// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicBullet.h"
#include "RicochetBullet.generated.h"

class AEnemyCharacter;

UCLASS()
class OSMK_API ARicochetBullet : public ABulletBase
{
	GENERATED_BODY()

public:
	ARicochetBullet();

protected:
	virtual void BeginPlay() override;

	virtual void OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	                         const FHitResult& Hit) override;

	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                            bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

private:
	void RegisterEnemyHit(AActor* OtherActor);

	FVector ClampUpwardDirection(const FVector& Direction, const FVector& ImpactVelocity) const;

	AEnemyCharacter* FindRicochetTarget(const FVector& Origin, const FVector& BounceDirection, const AActor* IgnoreActor) const;

	UPROPERTY(EditDefaultsOnly, Category = "Ricochet")
	float AimAssistRadius = 1500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Ricochet", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float AimAssistAngle = 25.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Ricochet", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AimAssistStrength = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Ricochet", meta = (ClampMin = "0.0", ClampMax = "90.0"))
	float MaxUpwardAngle = 15.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Ricochet", meta = (ClampMin = "1"))
	int32 MaxEnemyHitCount = 2;

	TSet<TWeakObjectPtr<AActor>> HitEnemies;
};
