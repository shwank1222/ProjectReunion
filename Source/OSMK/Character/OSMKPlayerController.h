// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OSMKPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class OSMK_API AOSMKPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AOSMKPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
#pragma region Input

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;
	
#pragma endregion
};
