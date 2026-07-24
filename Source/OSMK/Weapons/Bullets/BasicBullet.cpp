// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicBullet.h"


ABasicBullet::ABasicBullet()
{
}


void ABasicBullet::OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnBulletHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	
	Destroy();
}
