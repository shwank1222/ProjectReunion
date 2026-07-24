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
	
	void ActivateLogic();
	void DeactivateLogic(const FString& Reason) const;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponent;
	
private:
	UFUNCTION()
	void HandleTargetDeath();
	
	UFUNCTION()
	void HandleCharacterDeath();
};
