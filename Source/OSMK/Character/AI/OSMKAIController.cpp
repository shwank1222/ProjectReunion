// Fill out your copyright notice in the Description page of Project Settings.


#include "OSMKAIController.h"

#include "EnemyCharacter.h"
#include "Components/StateTreeAIComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AOSMKAIController::AOSMKAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	StateTreeAIComponent = CreateDefaultSubobject<UStateTreeAIComponent>("StateTreeAIComponent");
	StateTreeAIComponent->SetStartLogicAutomatically(false);
}

void AOSMKAIController::BeginPlay()
{
	Super::BeginPlay();
	
	ActivateLogic();
}

void AOSMKAIController::ActivateLogic()
{
	StateTreeAIComponent->StartLogic();
	
	if (ACharacter* Target = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		SetFocus(Target);
		
		if (AOSMKCharacterBase* Player = Cast<AOSMKCharacterBase>(Target))
		{
			Player->OnCharacterDeath.AddUniqueDynamic(this, &ThisClass::AOSMKAIController::HandleTargetDeath);
		}
	}
}

void AOSMKAIController::DeactivateLogic(const FString& Reason) const
{
	StateTreeAIComponent->StopLogic(Reason);
}

void AOSMKAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(InPawn))
	{
		Enemy->OnCharacterDeath.AddUniqueDynamic(this, &ThisClass::HandleCharacterDeath);
	}
}

void AOSMKAIController::HandleTargetDeath()
{
	ClearFocus(0);
	
	DeactivateLogic(TEXT("Player Dead"));
}

void AOSMKAIController::HandleCharacterDeath()
{
	ClearFocus(0);
}
