// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletBase.h"
#include "PiercingBullet.generated.h"

UCLASS()
class OSMK_API APiercingBullet : public ABulletBase
{
	GENERATED_BODY()

public:
	APiercingBullet();
	
protected:
	virtual void BeginPlay() override;
	
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	
	UPROPERTY(EditDefaultsOnly)
	int32 MaxPiercingCount = 2;
	
	int32 RemainingPiercingCount = 2;
};
