#pragma once

#include "CoreMinimal.h"
#include "SettingsTabWidgetBase.h"
#include "SettingsSoundWidget.generated.h"

struct FSoundBackup
{
	float MasterVolume = 1.0f;
	float BGMVolume    = 1.0f;
	float SFXVolume    = 1.0f;
	float UIVolume     = 1.0f;
	float VoiceVolume  = 1.0f;
};

UCLASS()
class OSMK_API USettingsSoundWidget : public USettingsTabWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeOnOpenTab(class UOSMKSaveGame* InSaveGame) override;
	virtual void NativeOnConfirmSettings(class UOSMKSaveGame* OutSaveGame) override;
	virtual void NativeOnCancelSettings() override;
	virtual void NativeOnResetToDefault() override;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnMasterVolumeChanged(float Value);

	UFUNCTION()
	void OnBGMVolumeChanged(float Value);

	UFUNCTION()
	void OnSFXVolumeChanged(float Value);

	UFUNCTION()
	void OnUIVolumeChanged(float Value);

	UFUNCTION()
	void OnVoiceVolumeChanged(float Value);

private:
	void ApplyVolume(class USoundClass* SoundClass, float Volume);
	void RestoreBackup();

protected:
	UPROPERTY(BlueprintReadWrite, Category="UI", meta=(BindWidget))
	class USliderWidget* MasterVolumeSlider = nullptr;

	UPROPERTY(BlueprintReadWrite, Category="UI", meta=(BindWidget))
	class USliderWidget* BGMVolumeSlider = nullptr;

	UPROPERTY(BlueprintReadWrite, Category="UI", meta=(BindWidget))
	class USliderWidget* SFXVolumeSlider = nullptr;

	UPROPERTY(BlueprintReadWrite, Category="UI", meta=(BindWidget))
	class USliderWidget* UIVolumeSlider = nullptr;

	UPROPERTY(BlueprintReadWrite, Category="UI", meta=(BindWidget))
	class USliderWidget* VoiceVolumeSlider = nullptr;

private:
	FSoundBackup BackupSettings;
};