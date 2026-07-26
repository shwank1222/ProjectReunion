#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "InputAction.h"
#include "OSMKSaveGame.generated.h"

UCLASS()
class OSMK_API UOSMKSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UOSMKSaveGame();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Sound")
	float MasterVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Sound")
	float BGMVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Sound")
	float SFXVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Sound")
	float UIVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Sound")
	float VoiceVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Control")
	float MouseSensitivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Language")
	FString CurrentCulture = TEXT("en");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Graphic")
	float Brightness = 0.5f;
};