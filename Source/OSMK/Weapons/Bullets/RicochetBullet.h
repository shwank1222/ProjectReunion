// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicBullet.h"
#include "RicochetBullet.generated.h"

UCLASS()
class OSMK_API ARicochetBullet : public ABasicBullet
{
	GENERATED_BODY()

public:
	ARicochetBullet();

protected:
	virtual void OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                         FVector NormalImpulse, const FHitResult& Hit) override;
};
