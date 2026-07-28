#include "SettingsGraphicWidget.h"
#include "GameFramework/GameUserSettings.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "SliderWidget.h"
#include "Core/OSMKSaveGame.h"
#include "Core/OSMKGameInstance.h"

void USettingsGraphicWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ScreenModeComboBox)
	{
		ScreenModeComboBox->ClearOptions();
		ScreenModeComboBox->AddOption(TEXT("Fullscreen"));
		ScreenModeComboBox->AddOption(TEXT("Windowed Fullscreen"));
		ScreenModeComboBox->AddOption(TEXT("Windowed"));
		ScreenModeComboBox->OnSelectionChanged.AddDynamic(this, &USettingsGraphicWidget::OnScreenModeChanged);
	}

	if (ResolutionComboBox)
	{
		ResolutionComboBox->ClearOptions();
		ResolutionComboBox->AddOption(TEXT("1280 × 720"));
		ResolutionComboBox->AddOption(TEXT("1600 × 900"));
		ResolutionComboBox->AddOption(TEXT("1920 × 1080"));
		ResolutionComboBox->AddOption(TEXT("2560 × 1440"));
		ResolutionComboBox->AddOption(TEXT("3440 × 1440"));
		ResolutionComboBox->AddOption(TEXT("3840 × 2160"));
		ResolutionComboBox->OnSelectionChanged.AddDynamic(this, &USettingsGraphicWidget::OnResolutionChanged);
	}

	QualityNormalStyles.Reset();
	for (UButton* Btn : { QualityButton_Highest, QualityButton_High, QualityButton_Medium, QualityButton_Low })
	{
		QualityNormalStyles.Add(Btn ? Btn->GetStyle() : FButtonStyle());
	}

	if (QualityButton_Highest)
	{
		QualityButton_Highest->OnClicked.AddDynamic(this, &USettingsGraphicWidget::OnQualityHighestClicked);
	}
	if (QualityButton_High)
	{
		QualityButton_High->OnClicked.AddDynamic(this, &USettingsGraphicWidget::OnQualityHighClicked);
	}
	if (QualityButton_Medium)
	{
		QualityButton_Medium->OnClicked.AddDynamic(this, &USettingsGraphicWidget::OnQualityMediumClicked);
	}
	if (QualityButton_Low)
	{
		QualityButton_Low->OnClicked.AddDynamic(this, &USettingsGraphicWidget::OnQualityLowClicked);
	}
	if (BrightnessSlider)
	{
		BrightnessSlider->OnValueChanged.AddDynamic(this, &USettingsGraphicWidget::OnBrightnessValueChanged);
	}
}

void USettingsGraphicWidget::NativeOnOpenTab(UOSMKSaveGame* InSaveGame)
{
	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	if (!UserSettings)
	{
		return;
	}

	BackupSettings.WindowMode     = UserSettings->GetFullscreenMode();
	BackupSettings.Resolution     = UserSettings->GetScreenResolution();
	BackupSettings.OverallQuality = UserSettings->GetOverallScalabilityLevel();
	BackupSettings.Brightness     = InSaveGame ? InSaveGame->Brightness : 0.5f;

	if (BrightnessSlider)
	{
		BrightnessSlider->SetValue(BackupSettings.Brightness);
	}

	UpdateUIFromUserSettings();
}

void USettingsGraphicWidget::UpdateUIFromUserSettings()
{
	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	if (!UserSettings)
	{
		return;
	}

	if (ScreenModeComboBox)
	{
		switch (UserSettings->GetFullscreenMode())
		{
		case EWindowMode::Fullscreen:
			ScreenModeComboBox->SetSelectedIndex(0); break;
		case EWindowMode::WindowedFullscreen: 
			ScreenModeComboBox->SetSelectedIndex(1); break;
		default: 
			ScreenModeComboBox->SetSelectedIndex(2); break;
		}
	}

	if (ResolutionComboBox)
	{
		FIntPoint CurrentRes = UserSettings->GetScreenResolution();
		ResolutionComboBox->SetSelectedOption(FString::Printf(TEXT("%d × %d"), CurrentRes.X, CurrentRes.Y));
	}

	PendingQuality = UserSettings->GetOverallScalabilityLevel();
	UpdateQualityButtonStyles();
}

void USettingsGraphicWidget::UpdateQualityButtonStyles()
{
	UButton* Buttons[] = { QualityButton_Highest, QualityButton_High, QualityButton_Medium, QualityButton_Low };

	for (int32 i = 0; i < 4; ++i)
	{
		if (!Buttons[i] || !QualityNormalStyles.IsValidIndex(i))
		{
			continue;
		}

		if (PendingQuality == (3 - i))
		{
			FButtonStyle SelectedStyle = QualityNormalStyles[i];
			SelectedStyle.Normal.TintColor  = FSlateColor(QualitySelectedColor);
			SelectedStyle.Hovered.TintColor = FSlateColor(QualitySelectedColor);
			Buttons[i]->SetStyle(SelectedStyle);
		}
		else
		{
			Buttons[i]->SetStyle(QualityNormalStyles[i]);
		}
	}
}

void USettingsGraphicWidget::ApplyQuality(int32 QualityLevel)
{
	PendingQuality = QualityLevel;
	UpdateQualityButtonStyles();

	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	if (!UserSettings)
	{
		return;
	}

	UserSettings->SetOverallScalabilityLevel(QualityLevel);
	UserSettings->ApplySettings(false);
}

void USettingsGraphicWidget::OnQualityHighestClicked()
{
	ApplyQuality(3);
}

void USettingsGraphicWidget::OnQualityHighClicked()
{
	ApplyQuality(2);
}

void USettingsGraphicWidget::OnQualityMediumClicked()
{
	ApplyQuality(1);
}

void USettingsGraphicWidget::OnQualityLowClicked()
{
	ApplyQuality(0);
}

void USettingsGraphicWidget::OnScreenModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	if (!UserSettings || !ScreenModeComboBox)
	{
		return;
	}

	switch (ScreenModeComboBox->GetSelectedIndex())
	{
	case 0: 
		UserSettings->SetFullscreenMode(EWindowMode::Fullscreen); break;
	case 1: 
		UserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen); break;
	default: 
		UserSettings->SetFullscreenMode(EWindowMode::Windowed); break;
	}

	UserSettings->ApplySettings(false);
}

void USettingsGraphicWidget::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	if (!UserSettings)
	{
		return;
	}

	FString LeftStr, RightStr;
	if (SelectedItem.Split(TEXT("×"), &LeftStr, &RightStr))
	{
		int32 Width  = FCString::Atoi(*LeftStr.TrimStartAndEnd());
		int32 Height = FCString::Atoi(*RightStr.TrimStartAndEnd());

		if (Width > 0 && Height > 0)
		{
			UserSettings->SetScreenResolution(FIntPoint(Width, Height));
			UserSettings->ConfirmVideoMode();
			UserSettings->ApplySettings(false);
			FSlateApplication::Get().InvalidateAllWidgets(true);
		}
	}
}

void USettingsGraphicWidget::ApplyBrightness(float Value)
{
	if (!GEngine)
	{
		return;
	}

	UOSMKGameInstance* GI = GetGameInstance<UOSMKGameInstance>();
	if (!GI)
	{
		return;
	}

	GEngine->DisplayGamma = FMath::Lerp(GI->BrightnessGammaMin, GI->BrightnessGammaMax, Value);
}

void USettingsGraphicWidget::OnBrightnessValueChanged(float Value)
{
	ApplyBrightness(Value);
}

void USettingsGraphicWidget::NativeOnConfirmSettings(UOSMKSaveGame* OutSaveGame)
{
	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	if (UserSettings)
	{
		UserSettings->SaveSettings();
	}

	if (OutSaveGame && BrightnessSlider)
	{
		OutSaveGame->Brightness = BrightnessSlider->GetValue();
	}
}

void USettingsGraphicWidget::NativeOnCancelSettings()
{
	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	if (!UserSettings)
	{
		return;
	}

	UserSettings->SetFullscreenMode(BackupSettings.WindowMode);
	UserSettings->SetScreenResolution(BackupSettings.Resolution);
	UserSettings->SetOverallScalabilityLevel(BackupSettings.OverallQuality);
	UserSettings->ApplySettings(false);

	PendingQuality = BackupSettings.OverallQuality;
	UpdateQualityButtonStyles();

	if (BrightnessSlider)
	{
		BrightnessSlider->SetValue(BackupSettings.Brightness);
	}

	ApplyBrightness(BackupSettings.Brightness);
}

void USettingsGraphicWidget::NativeOnResetToDefault()
{
	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	if (!UserSettings)
	{
		return;
	}

	UserSettings->SetToDefaults();
	UserSettings->SetOverallScalabilityLevel(3);
	UserSettings->ApplySettings(false);

	const float DefaultBrightness = GetDefault<UOSMKSaveGame>()->Brightness;

	if (BrightnessSlider)
	{
		BrightnessSlider->SetValue(DefaultBrightness);
	}

	ApplyBrightness(DefaultBrightness);

	UpdateUIFromUserSettings();
}