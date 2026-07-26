#include "SettingsSoundWidget.h"
#include "SliderWidget.h"
#include "Core/OSMKGameInstance.h"
#include "Core/OSMKSaveGame.h"

void USettingsSoundWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (MasterVolumeSlider)
	{
		MasterVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsSoundWidget::OnMasterVolumeChanged);
	}
	if (BGMVolumeSlider)
	{
		BGMVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsSoundWidget::OnBGMVolumeChanged);
	}
	if (SFXVolumeSlider)
	{
		SFXVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsSoundWidget::OnSFXVolumeChanged);
	}
	if (UIVolumeSlider)
	{
		UIVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsSoundWidget::OnUIVolumeChanged);
	}
	if (VoiceVolumeSlider)
	{
		VoiceVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsSoundWidget::OnVoiceVolumeChanged);
	}
}

void USettingsSoundWidget::NativeOnOpenTab(UOSMKSaveGame* InSaveGame)
{
	if (!InSaveGame)
	{
		return;
	}

	BackupSettings = { InSaveGame->MasterVolume, InSaveGame->BGMVolume, InSaveGame->SFXVolume, InSaveGame->UIVolume, InSaveGame->VoiceVolume };

	if (MasterVolumeSlider)
	{
		MasterVolumeSlider->SetValue(InSaveGame->MasterVolume);
	}
	if (BGMVolumeSlider)
	{
		BGMVolumeSlider->SetValue(InSaveGame->BGMVolume);
	}
	if (SFXVolumeSlider)
	{
		SFXVolumeSlider->SetValue(InSaveGame->SFXVolume);
	}
	if (UIVolumeSlider)
	{
		UIVolumeSlider->SetValue(InSaveGame->UIVolume);
	}
	if (VoiceVolumeSlider)
	{
		VoiceVolumeSlider->SetValue(InSaveGame->VoiceVolume);
	}
}

void USettingsSoundWidget::NativeOnConfirmSettings(UOSMKSaveGame* OutSaveGame)
{
	if (!OutSaveGame)
	{
		return;
	}

	if (MasterVolumeSlider)
	{
		OutSaveGame->MasterVolume = MasterVolumeSlider->GetValue();
	}
	if (BGMVolumeSlider)
	{
		OutSaveGame->BGMVolume = BGMVolumeSlider->GetValue();
	}
	if (SFXVolumeSlider)
	{
		OutSaveGame->SFXVolume = SFXVolumeSlider->GetValue();
	}
	if (UIVolumeSlider)
	{
		OutSaveGame->UIVolume = UIVolumeSlider->GetValue();
	}
	if (VoiceVolumeSlider)
	{
		OutSaveGame->VoiceVolume = VoiceVolumeSlider->GetValue();
	}
}

void USettingsSoundWidget::NativeOnCancelSettings()
{
	RestoreBackup();
}

void USettingsSoundWidget::NativeOnResetToDefault()
{
	constexpr float DefaultVolume = 1.0f;

	if (MasterVolumeSlider)
	{
		MasterVolumeSlider->SetValue(DefaultVolume);
	}
	if (BGMVolumeSlider)
	{
		BGMVolumeSlider->SetValue(DefaultVolume);
	}
	if (SFXVolumeSlider)
	{
		SFXVolumeSlider->SetValue(DefaultVolume);
	}
	if (UIVolumeSlider)
	{
		UIVolumeSlider->SetValue(DefaultVolume);
	}
	if (VoiceVolumeSlider)
	{
		VoiceVolumeSlider->SetValue(DefaultVolume);
	}

	if (UOSMKGameInstance* GI = Cast<UOSMKGameInstance>(GetGameInstance()))
	{
		GI->ApplySoundClassVolume(GI->SC_Master, DefaultVolume);
		GI->ApplySoundClassVolume(GI->SC_BGM,    DefaultVolume);
		GI->ApplySoundClassVolume(GI->SC_SFX,    DefaultVolume);
		GI->ApplySoundClassVolume(GI->SC_UI,     DefaultVolume);
		GI->ApplySoundClassVolume(GI->SC_Voice,  DefaultVolume);
	}
}

void USettingsSoundWidget::OnMasterVolumeChanged(float Value)
{
	if (UOSMKGameInstance* GI = Cast<UOSMKGameInstance>(GetGameInstance()))
	{
		ApplyVolume(GI->SC_Master, Value);
	}
}

void USettingsSoundWidget::OnBGMVolumeChanged(float Value)
{
	if (UOSMKGameInstance* GI = Cast<UOSMKGameInstance>(GetGameInstance()))
	{
		ApplyVolume(GI->SC_BGM, Value);
	}
}

void USettingsSoundWidget::OnSFXVolumeChanged(float Value)
{
	if (UOSMKGameInstance* GI = Cast<UOSMKGameInstance>(GetGameInstance()))
	{
		ApplyVolume(GI->SC_SFX, Value);
	}
}

void USettingsSoundWidget::OnUIVolumeChanged(float Value)
{
	if (UOSMKGameInstance* GI = Cast<UOSMKGameInstance>(GetGameInstance()))
	{
		ApplyVolume(GI->SC_UI, Value);
	}
}

void USettingsSoundWidget::OnVoiceVolumeChanged(float Value)
{
	if (UOSMKGameInstance* GI = Cast<UOSMKGameInstance>(GetGameInstance()))
	{
		ApplyVolume(GI->SC_Voice, Value);
	}
}

void USettingsSoundWidget::ApplyVolume(USoundClass* SoundClass, float Volume)
{
	if (UOSMKGameInstance* GI = Cast<UOSMKGameInstance>(GetGameInstance()))
	{
		GI->ApplySoundClassVolume(SoundClass, Volume);
	}
}

void USettingsSoundWidget::RestoreBackup()
{
	if (MasterVolumeSlider)
	{
		MasterVolumeSlider->SetValue(BackupSettings.MasterVolume);
	}
	if (BGMVolumeSlider)
	{
		BGMVolumeSlider->SetValue(BackupSettings.BGMVolume);
	}
	if (SFXVolumeSlider)
	{
		SFXVolumeSlider->SetValue(BackupSettings.SFXVolume);
	}
	if (UIVolumeSlider)
	{
		UIVolumeSlider->SetValue(BackupSettings.UIVolume);
	}
	if (VoiceVolumeSlider)
	{
		VoiceVolumeSlider->SetValue(BackupSettings.VoiceVolume);
	}
}