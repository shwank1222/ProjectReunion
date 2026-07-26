// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OSMKPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UPauseMenuWidget;

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

	UPROPERTY(EditDefaultsOnly, Category = "Input|Scouting")
	TObjectPtr<UInputMappingContext> ScoutingMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Scouting")
	TObjectPtr<UInputAction> IA_ScoutRotate = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Scouting")
	TObjectPtr<UInputAction> IA_ScoutZoom = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Pause = nullptr;

#pragma endregion

#pragma region Pause Menu

public:
	void TogglePauseMenu();
	
private:
	void OnPausePressed(const struct FInputActionValue& Value);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPauseMenuWidget> PauseMenuWidgetClass = nullptr;

private:
	UPROPERTY()
	TObjectPtr<UPauseMenuWidget> PauseMenuWidgetInstance = nullptr;

#pragma endregion

#pragma region Scouting Camera
public:
	void EnterScoutingMode(AActor* CameraActor);
	void ExitScoutingMode();
	
private:
	void OnScoutRotate(const struct FInputActionValue& Value);
	void OnScoutZoom(const struct FInputActionValue& Value);

private:
	UPROPERTY()
	AActor* ScoutCameraActor = nullptr;
#pragma endregion
};
