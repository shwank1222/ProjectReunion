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
	BackupSlowFireSensitivity = InSaveGame->SlowFireSensitivity;

	if (SensitivitySlider)
	{
		SensitivitySlider->SetValue(BackupSensitivity);
	}
	if (SlowFireSensitivitySlider)
	{
		SlowFireSensitivitySlider->SetValue(BackupSlowFireSensitivity);
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
	if (SlowFireSensitivitySlider)
	{
		OutSaveGame->SlowFireSensitivity = SlowFireSensitivitySlider->GetValue();
	}
}

void USettingsControlWidget::NativeOnCancelSettings()
{
	ApplySensitivitySettings(BackupSensitivity);

	if (SlowFireSensitivitySlider)
	{
		SlowFireSensitivitySlider->SetValue(BackupSlowFireSensitivity);
	}
}

void USettingsControlWidget::NativeOnResetToDefault()
{
	constexpr float DefaultSensitivity = 1.0f;
	const float DefaultSlowFireSensitivity = GetDefault<UOSMKSaveGame>()->SlowFireSensitivity;

	if (SensitivitySlider)
	{
		SensitivitySlider->SetValue(DefaultSensitivity);
	}
	if (SlowFireSensitivitySlider)
	{
		SlowFireSensitivitySlider->SetValue(DefaultSlowFireSensitivity);
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