#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OSMKGameInstance.generated.h"

UCLASS()
class OSMK_API UOSMKGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	void LoadGameSettings();
	void SaveGameSettings();
	void ApplySoundSettings() const;
	void ApplyBrightnessSettings() const;
	void ApplySoundClassVolume(class USoundClass* SoundClass, float Volume) const;

	FORCEINLINE class UOSMKSaveGame* GetSettingSaveGame() const { return SettingSaveData; }

public:
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundMix* MasterSoundMix = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundData* SoundData = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundClass* SC_Master = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundClass* SC_BGM = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundClass* SC_SFX = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundClass* SC_UI = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundClass* SC_Voice = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Graphics")
	float BrightnessGammaMin = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Graphics")
	float BrightnessGammaMax = 3.0f;

private:
	UPROPERTY()
	TObjectPtr<class UOSMKSaveGame> SettingSaveData = nullptr;

	const FString SettingsSlotName = TEXT("OSMKSettingsSlot");
	const int32 SettingsUserIndex = 0;
};