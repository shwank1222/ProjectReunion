#include "SettingsControlWidget.h"
#include "SliderWidget.h"
#include "Core/OSMKSaveGame.h"

void USettingsControlWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SensitivitySlider)
	{
		SensitivitySlider->OnValueChanged.AddDynamic(this, &USettingsControlWidget::OnSensitivityChanged);
	}
}

void USettingsControlWidget::NativeOnOpenTab(UOSMKSaveGame* InSaveGame)
{
	if (!InSaveGame)
	{
		return;
	}

	BackupSensitivity = InSaveGame->MouseSensitivity;

	if (SensitivitySlider)
	{
		SensitivitySlider->SetValue(BackupSensitivity);
	}
}

void USettingsControlWidget::NativeOnConfirmSettings(UOSMKSaveGame* OutSaveGame)
{
	if (!OutSaveGame)
	{
		return;
	}

	if (SensitivitySlider)
	{
		OutSaveGame->MouseSensitivity = SensitivitySlider->GetValue();
	}
}

void USettingsControlWidget::NativeOnCancelSettings()
{
	ApplySensitivitySettings(BackupSensitivity);
}

void USettingsControlWidget::NativeOnResetToDefault()
{
	constexpr float DefaultSensitivity = 1.0f;

	if (SensitivitySlider)
	{
		SensitivitySlider->SetValue(DefaultSensitivity);
	}

	ApplySensitivitySettings(DefaultSensitivity);
}

void USettingsControlWidget::OnSensitivityChanged(float Value)
{
	ApplySensitivitySettings(Value);
}

void USettingsControlWidget::ApplySensitivitySettings(float Value)
{
	if (SensitivitySlider)
	{
		SensitivitySlider->SetValue(Value);
	}
}