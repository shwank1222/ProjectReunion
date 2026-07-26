// Fill out your copyright notice in the Description page of Project Settings.


#include "OSMKPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/SpringArmComponent.h"
#include "UI/Ingame/PauseMenuWidget.h"

class UEnhancedInputLocalPlayerSubsystem;

AOSMKPlayerController::AOSMKPlayerController()
{
}

void AOSMKPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(PlayerCameraManager))
	{
		PlayerCameraManager->ViewPitchMin = -60.0f;
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

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (IA_Pause)
		{
			EIC->BindAction(IA_Pause, ETriggerEvent::Started, this, &AOSMKPlayerController::OnPausePressed);
		}
	}
}

void AOSMKPlayerController::TogglePauseMenu()
{
	if (PauseMenuWidgetInstance && PauseMenuWidgetInstance->IsInViewport())
	{
		PauseMenuWidgetInstance->RemoveFromParent();
		PauseMenuWidgetInstance = nullptr;
		return;
	}

	if (!PauseMenuWidgetClass)
	{
		return;
	}

	PauseMenuWidgetInstance = CreateWidget<UPauseMenuWidget>(this, PauseMenuWidgetClass);
	if (PauseMenuWidgetInstance)
	{
		PauseMenuWidgetInstance->AddToViewport();
		PauseMenuWidgetInstance->SetFocus();
	}
}

void AOSMKPlayerController::OnPausePressed(const FInputActionValue& Value)
{
	TogglePauseMenu();
}

void AOSMKPlayerController::EnterScoutingMode(AActor* CameraActor)
{
	ScoutCameraActor = CameraActor;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem && ScoutingMappingContext)
	{
		Subsystem->AddMappingContext(ScoutingMappingContext, 1);
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (EIC)
	{
		if (IA_ScoutRotate)
		{
			EIC->BindAction(IA_ScoutRotate, ETriggerEvent::Triggered, this, &AOSMKPlayerController::OnScoutRotate);
		}
		if (IA_ScoutZoom)
		{
			EIC->BindAction(IA_ScoutZoom, ETriggerEvent::Triggered, this, &AOSMKPlayerController::OnScoutZoom);
		}
	}

	bShowMouseCursor = true;
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}

void AOSMKPlayerController::ExitScoutingMode()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem && ScoutingMappingContext)
	{
		Subsystem->RemoveMappingContext(ScoutingMappingContext);
	}

	ScoutCameraActor = nullptr;

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void AOSMKPlayerController::OnScoutRotate(const FInputActionValue& Value)
{
	if (!ScoutCameraActor)
	{
		return;
	}

	USpringArmComponent* SpringArm = ScoutCameraActor->FindComponentByClass<USpringArmComponent>();
	if (!SpringArm)
	{
		return;
	}

	FVector2D Input = Value.Get<FVector2D>();
	FRotator CurrentRot = ScoutCameraActor->GetActorRotation();
	CurrentRot.Yaw = FMath::Clamp(CurrentRot.Yaw + Input.X, -60.f, 60.f);
	ScoutCameraActor->SetActorRotation(CurrentRot);
}

void AOSMKPlayerController::OnScoutZoom(const FInputActionValue& Value)
{
	if (!ScoutCameraActor)
	{
		return;
	}

	USpringArmComponent* SpringArm = ScoutCameraActor->FindComponentByClass<USpringArmComponent>();
	if (!SpringArm)
	{
		return;
	}

	float Input = Value.Get<float>();
	SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength - Input * 100.f, 200.f, 5000.f);
}
