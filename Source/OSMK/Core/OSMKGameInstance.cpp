#include "OSMKGameInstance.h"
#include "OSMKSaveGame.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"

void UOSMKGameInstance::Init()
{
	Super::Init();

	LoadGameSettings();

	if (UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
	{
		UserSettings->LoadSettings();
		UserSettings->ApplySettings(false);
	}
}

void UOSMKGameInstance::LoadGameSettings()
{
	if (UGameplayStatics::DoesSaveGameExist(SettingsSlotName, SettingsUserIndex))
	{
		SettingSaveData = Cast<UOSMKSaveGame>(UGameplayStatics::LoadGameFromSlot(SettingsSlotName, SettingsUserIndex));
	}

	if (!SettingSaveData)
	{
		SettingSaveData = Cast<UOSMKSaveGame>(UGameplayStatics::CreateSaveGameObject(UOSMKSaveGame::StaticClass()));
		SaveGameSettings();
	}

	ApplySoundSettings();
	ApplyBrightnessSettings();
}

void UOSMKGameInstance::ApplySoundSettings() const
{
	if (!SettingSaveData || !MasterSoundMix)
	{
		return;
	}

	UGameplayStatics::PushSoundMixModifier(this, MasterSoundMix);

	ApplySoundClassVolume(SC_Master, SettingSaveData->MasterVolume);
	ApplySoundClassVolume(SC_BGM,    SettingSaveData->BGMVolume);
	ApplySoundClassVolume(SC_SFX,    SettingSaveData->SFXVolume);
	ApplySoundClassVolume(SC_UI,     SettingSaveData->UIVolume);
	ApplySoundClassVolume(SC_Voice,  SettingSaveData->VoiceVolume);
}

void UOSMKGameInstance::ApplySoundClassVolume(USoundClass* SoundClass, float Volume) const
{
	if (!MasterSoundMix || !SoundClass)
	{
		return;
	}

	UGameplayStatics::PushSoundMixModifier(this, MasterSoundMix);
	UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix, SoundClass, Volume, 1.0f, 0.0f);
}

void UOSMKGameInstance::ApplyBrightnessSettings() const
{
	if (!SettingSaveData || !GEngine)
	{
		return;
	}

	GEngine->DisplayGamma = FMath::Lerp(BrightnessGammaMax, BrightnessGammaMin, SettingSaveData->Brightness);
}

void UOSMKGameInstance::SaveGameSettings()
{
	if (SettingSaveData)
	{
		UGameplayStatics::SaveGameToSlot(SettingSaveData, SettingsSlotName, SettingsUserIndex);
	}
}
