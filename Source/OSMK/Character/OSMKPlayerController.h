// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OSMKPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class OSMK_API AOSMKPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOSMKPlayerController();

	void EnterScoutingMode(AActor* CameraActor);
	void ExitScoutingMode();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

#pragma region Input

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Scouting")
	TObjectPtr<UInputMappingContext> ScoutingMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Scouting")
	TObjectPtr<UInputAction> IA_ScoutRotate;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Scouting")
	TObjectPtr<UInputAction> IA_ScoutZoom;

#pragma endregion

#pragma region Scouting Camera
private:
	void OnScoutRotate(const struct FInputActionValue& Value);
	void OnScoutZoom(const struct FInputActionValue& Value);
	
private:
	UPROPERTY()
	AActor* ScoutCameraActor = nullptr;
#pragma endregion
};
