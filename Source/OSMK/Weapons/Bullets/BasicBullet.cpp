// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicBullet.h"

#include "Interactable/Gimmick/GimmickBase.h"


ABasicBullet::ABasicBullet()
{
	MeshComponent->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnBulletHit);
}


void ABasicBullet::OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogBullet, Warning, TEXT("Basic Bullet hit!"));
	
	if (IsValid(OtherActor))
	{
		UE_LOG(LogBullet, Warning, TEXT("Hit Actor: %s"), *OtherActor->GetName());
	}
	
	TriggerGimmick(OtherActor);
	
	Destroy();
}
