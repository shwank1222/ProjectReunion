// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletBase.h"
#include "BasicBullet.generated.h"

UCLASS()
class OSMK_API ABasicBullet : public ABulletBase
{
	GENERATED_BODY()

public:
	ABasicBullet();
	
protected:
	UFUNCTION()
	virtual void OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
