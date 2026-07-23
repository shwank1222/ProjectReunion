// Fill out your copyright notice in the Description page of Project Settings.


#include "PiercingBullet.h"


APiercingBullet::APiercingBullet()
{
	MeshComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &APiercingBullet::OnBeginOverlap);
}

void APiercingBullet::BeginPlay()
{
	Super::BeginPlay();
	
	RemainingPiercingCount = MaxPiercingCount;
}

void APiercingBullet::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogBullet, Warning, TEXT("Piercing Bullet Overlap!"));
	
	if (IsValid(OtherActor))
	{
		UE_LOG(LogBullet, Warning, TEXT("Overlap Actor: %s"), *OtherActor->GetName());
	}
	
	TriggerGimmick(OtherActor);
	
	RemainingPiercingCount--;
	
	UE_LOG(LogBullet, Warning, TEXT("Remaining Piercing Count: %d"), RemainingPiercingCount);
	
	if (RemainingPiercingCount <= 0)
	{
		Destroy();
	}
}

