// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "OSMKAIController.generated.h"

class UStateTreeAIComponent;

UCLASS()
class OSMK_API AOSMKAIController : public AAIController
{
	GENERATED_BODY()

public:
	AOSMKAIController();
	
	virtual void BeginPlay() override;
	
	void ActivateLogic();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponent;
};
