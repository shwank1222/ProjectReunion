// Fill out your copyright notice in the Description page of Project Settings.


#include "PiercingBullet.h"

#include "Character/AI/EnemyCharacter.h"


APiercingBullet::APiercingBullet()
{
}

void APiercingBullet::BeginPlay()
{
	Super::BeginPlay();
	
	RemainingPiercingCount = MaxPiercingCount;
}

void APiercingBullet::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	if (!Cast<AEnemyCharacter>(OtherActor))
	{
		Destroy();
		
		return;
	}
	
	RemainingPiercingCount--;
	
	UE_LOG(LogBullet, Warning, TEXT("Remaining Piercing Count: %d"), RemainingPiercingCount);
	
	if (RemainingPiercingCount <= 0)
	{
		Destroy();
	}
}

