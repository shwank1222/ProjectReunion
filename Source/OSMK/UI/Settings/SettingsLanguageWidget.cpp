#include "SettingsLanguageWidget.h"
#include "Components/ComboBoxString.h"
#include "Core/OSMKSaveGame.h"
#include "Kismet/KismetInternationalizationLibrary.h"

void USettingsLanguageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (LanguageComboBox)
	{
		LanguageComboBox->ClearOptions();
		LanguageComboBox->AddOption(TEXT("English"));

		LanguageComboBox->OnSelectionChanged.AddDynamic(this, &USettingsLanguageWidget::OnLanguageSelectionChanged);
	}
}

void USettingsLanguageWidget::NativeOnOpenTab(UOSMKSaveGame* InSaveGame)
{
	if (!InSaveGame)
	{
		return;
	}

	BackupCulture = InSaveGame->CurrentCulture;
	PendingCulture = BackupCulture;

	if (LanguageComboBox)
	{
		if (BackupCulture.StartsWith(TEXT("en")))
		{
			LanguageComboBox->SetSelectedOption(TEXT("English"));
		}
	}
}

void USettingsLanguageWidget::OnLanguageSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectedItem == TEXT("English"))
	{
		PendingCulture = TEXT("en");
	}
	else
	{
		PendingCulture = TEXT("ko");
	}

	ApplyLanguageSettings(PendingCulture);
}

void USettingsLanguageWidget::ApplyLanguageSettings(const FString& CultureCode)
{
	UKismetInternationalizationLibrary::SetCurrentCulture(CultureCode, false);
}

void USettingsLanguageWidget::NativeOnConfirmSettings(UOSMKSaveGame* OutSaveGame)
{
	if (!OutSaveGame)
	{
		return;
	}
	OutSaveGame->CurrentCulture = PendingCulture;
}

void USettingsLanguageWidget::NativeOnCancelSettings()
{
	PendingCulture = BackupCulture;
	ApplyLanguageSettings(BackupCulture);
}

void USettingsLanguageWidget::NativeOnResetToDefault()
{
	const FString DefaultCulture = TEXT("en");
	
	if (LanguageComboBox)
	{
		LanguageComboBox->SetSelectedOption(TEXT("English"));
	}

	PendingCulture = DefaultCulture;
	ApplyLanguageSettings(DefaultCulture);
}