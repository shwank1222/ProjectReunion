// Fill out your copyright notice in the Description page of Project Settings.


#include "OSMKAIController.h"

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
	
	if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		SetFocus(Player);
	}
}
