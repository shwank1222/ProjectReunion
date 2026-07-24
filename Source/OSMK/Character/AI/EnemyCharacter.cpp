// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "OSMKAIController.h"
#include "Character/PlayerCharacter.h"
#include "Components/ArrowComponent.h"
#include "Core/OSMKGameState.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogEnemy, Log, All);

AEnemyCharacter::AEnemyCharacter()
{
	PistolMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PistolMesh"));
	PistolMesh->SetupAttachment(GetMesh(), FName("HandGrip_R"));

	AttackArrow = CreateDefaultSubobject<UArrowComponent>(FName("AttackArrow"));
	AttackArrow->SetupAttachment(RootComponent);
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	if (bAutoActivate)
	{
		ActivateEnemy();
	}
}

void AEnemyCharacter::Fire() const
{
	const FVector Start = AttackArrow->GetComponentLocation();
	const FVector Direction = AttackArrow->GetForwardVector();
	const FVector End = Start + Direction * MaxAimDistance;

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 5.0f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredComponent(PistolMesh.Get());

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		UE_LOG(LogEnemy, Warning, TEXT("Hit: %s"), *Hit.GetActor()->GetName());

		if (APlayerCharacter* Player = Cast<APlayerCharacter>(Hit.GetActor()))
		{
			Player->ApplyDamage();
		}
	}
}

void AEnemyCharacter::ActivateEnemy() const
{
	if (AOSMKAIController* AIController = Cast<AOSMKAIController>(GetController()))
	{
		AIController->ActivateLogic();
	}
}

void AEnemyCharacter::Die()
{
	Super::Die();

	if (AOSMKGameState* GS = GetWorld()->GetGameState<AOSMKGameState>())
	{
		GS->NotifyEnemyKilled();
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::DestroyCharacter, 2.0f, false);
}

void AEnemyCharacter::DestroyCharacter()
{
	if (const AOSMKAIController* AIController = Cast<AOSMKAIController>(GetController()))
	{
		AIController->DeactivateLogic(TEXT("Enemy Dead"));
	}

	Destroy();
}
