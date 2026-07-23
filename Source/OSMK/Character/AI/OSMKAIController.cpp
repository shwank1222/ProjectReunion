// Fill out your copyright notice in the Description page of Project Settings.


#include "OSMKAIController.h"

#include "Components/StateTreeAIComponent.h"

AOSMKAIController::AOSMKAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	StateTreeAIComponent = CreateDefaultSubobject<UStateTreeAIComponent>("StateTreeAIComponent");
	StateTreeAIComponent->SetStartLogicAutomatically(false);
}

void AOSMKAIController::ActivateLogic() const
{
	StateTreeAIComponent->StartLogic();
}
