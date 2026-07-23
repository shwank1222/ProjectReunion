// Fill out your copyright notice in the Description page of Project Settings.


#include "RicochetBullet.h"

#include "GameFramework/ProjectileMovementComponent.h"

ARicochetBullet::ARicochetBullet()
{
	ProjectileMovement->bShouldBounce = true;
}

void ARicochetBullet::OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogBullet, Warning, TEXT("Ricochet Bullet hit!"));
	
	if (IsValid(OtherActor))
	{
		UE_LOG(LogBullet, Warning, TEXT("Hit Actor: %s"), *OtherActor->GetName());
	}
}

