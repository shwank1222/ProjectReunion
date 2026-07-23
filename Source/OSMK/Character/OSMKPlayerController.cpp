// Fill out your copyright notice in the Description page of Project Settings.


#include "OSMKPlayerController.h"
#include "EnhancedInputSubsystems.h"

class UEnhancedInputLocalPlayerSubsystem;

AOSMKPlayerController::AOSMKPlayerController()
{
}

void AOSMKPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(PlayerCameraManager))
	{
		PlayerCameraManager->ViewPitchMin = - 70.0f;
		PlayerCameraManager->ViewPitchMax = 80.0f;
	}
}

void AOSMKPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (const UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
}
