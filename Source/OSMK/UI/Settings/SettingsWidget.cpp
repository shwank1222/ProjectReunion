#include "SettingsWidget.h"
#include "SettingsTabWidgetBase.h"
#include "SettingsGraphicWidget.h"
#include "SettingsSoundWidget.h"
#include "SettingsLanguageWidget.h"
#include "SettingsControlWidget.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Core/OSMKGameInstance.h"

void USettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &USettingsWidget::OnConfirmClicked);
	}
	if (ResetButton)
	{
		ResetButton->OnClicked.AddDynamic(this, &USettingsWidget::OnResetClicked);
	}
	if (CancelButton)
	{
		CancelButton->OnClicked.AddDynamic(this, &USettingsWidget::OnCancelClicked);
	}

	SettingTabs.Empty();
	if (GraphicTabWidget)
	{
		SettingTabs.Add(GraphicTabWidget);
	}
	
	if (SoundTabWidget)
	{
		SettingTabs.Add(SoundTabWidget);
	}
	if (ControlTabWidget)
	{
		SettingTabs.Add(ControlTabWidget);
	}
	if (LanguageTabWidget) 
	{
		SettingTabs.Add(LanguageTabWidget);
	}

	InitializeTabButtons();

	if (UOSMKGameInstance* GI = Cast<UOSMKGameInstance>(GetGameInstance()))
	{
		if (UOSMKSaveGame* SaveGame = GI->GetSettingSaveGame())
		{
			for (USettingsTabWidgetBase* Tab : SettingTabs)
			{
				if (Tab) Tab->NativeOnOpenTab(SaveGame);
			}
		}
	}

	SwitchToTab(0);
	SetIsFocusable(true);
	SetFocus();
}

void USettingsWidget::InitializeTabButtons()
{
	TabButtons.Empty();

	if (!TabButtonContainer)
	{
		return;
	}

	int32 ChildCount = TabButtonContainer->GetChildrenCount();
	for (int32 i = 0; i < ChildCount; ++i)
	{
		UWidget* DirectChild = TabButtonContainer->GetChildAt(i);
		if (!DirectChild)
		{
			continue;
		}

		if (UButton* TargetButton = Cast<UButton>(DirectChild))
		{
			TabButtons.Add(TargetButton);
			TargetButton->OnClicked.AddDynamic(this, &USettingsWidget::OnTabButtonClicked);
		}
		else if (UPanelWidget* SubPanel = Cast<UPanelWidget>(DirectChild))
		{
			int32 SubChildCount = SubPanel->GetChildrenCount();
			for (int32 j = 0; j < SubChildCount; ++j)
			{
				if (UButton* SubButton = Cast<UButton>(SubPanel->GetChildAt(j)))
				{
					TabButtons.Add(SubButton);
					SubButton->OnClicked.AddDynamic(this, &USettingsWidget::OnTabButtonClicked);
				}
			}
		}
	}
}

void USettingsWidget::OnTabButtonClicked()
{
	for (int32 i = 0; i < TabButtons.Num(); ++i)
	{
		if (TabButtons[i] && (TabButtons[i]->IsPressed() || TabButtons[i]->HasUserFocus(GetOwningPlayer())))
		{
			SwitchToTab(i);
			break;
		}
	}
}

void USettingsWidget::SwitchToTab(int32 TabIndex)
{
	if (!TabSwitcher || !SettingTabs.IsValidIndex(TabIndex))
	{
		return;
	}

	TabSwitcher->SetActiveWidgetIndex(TabIndex);

	for (int32 i = 0; i < TabButtons.Num(); ++i)
	{
		if (TabButtons[i])
		{
			TabButtons[i]->SetIsEnabled(i != TabIndex);
		}
	}
}

FReply USettingsWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		OnCancelClicked();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void USettingsWidget::OnConfirmClicked()
{
	if (UOSMKGameInstance* GI = Cast<UOSMKGameInstance>(GetGameInstance()))
	{
		if (UOSMKSaveGame* SaveGame = GI->GetSettingSaveGame())
		{
			for (USettingsTabWidgetBase* Tab : SettingTabs)
			{
				if (Tab)
				{
					Tab->NativeOnConfirmSettings(SaveGame);
				}
			}

			GI->SaveGameSettings();
		}
	}

	CloseSettingsWidget();
}

void USettingsWidget::OnResetClicked()
{
	if (!TabSwitcher)
	{
		return;
	}

	if (USettingsTabWidgetBase* ActiveTab = Cast<USettingsTabWidgetBase>(TabSwitcher->GetActiveWidget()))
	{
		ActiveTab->NativeOnResetToDefault();
	}
}

void USettingsWidget::OnCancelClicked()
{
	for (USettingsTabWidgetBase* Tab : SettingTabs)
	{
		if (Tab)
		{
			Tab->NativeOnCancelSettings();
		}
	}

	CloseSettingsWidget();
}

void USettingsWidget::CloseSettingsWidget()
{
	RemoveFromParent();
}