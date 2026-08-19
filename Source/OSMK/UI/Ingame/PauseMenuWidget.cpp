#include "PauseMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Title/PopupConfirmWidget.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;
	
	if (APlayerController* PC = GetOwningPlayer())
	{
		bRestoreCursorOnClose = PC->bShowMouseCursor;

		PC->SetPause(true);
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}

	if (Btn_Close)
	{
		Btn_Close->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnCloseClicked);
	}
	if (Btn_Settings)
	{
		Btn_Settings->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnSettingsClicked);
	}
	if (Btn_QuitToTitle)
	{
		Btn_QuitToTitle->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnQuitClicked);
	}
}

void UPauseMenuWidget::NativeDestruct()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetPause(false);

		if (bRestoreCursorOnClose)
		{
			PC->bShowMouseCursor = true;

			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			PC->SetInputMode(InputMode);
		}
		else
		{
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}

		PC->FlushPressedKeys();
	}

	Super::NativeDestruct();
}

FReply UPauseMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		OnCloseClicked();
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UPauseMenuWidget::OnCloseClicked()
{
	RemoveFromParent();
}

void UPauseMenuWidget::OnSettingsClicked()
{
	if (SettingsWidgetClass)
	{
		UUserWidget* SettingsWidget = CreateWidget<UUserWidget>(this, SettingsWidgetClass);
		if (SettingsWidget)
		{
			SettingsWidget->AddToViewport();
		}
	}
}

void UPauseMenuWidget::OnQuitClicked()
{
	if (PopupConfirmWidgetClass)
	{
		UPopupConfirmWidget* Popup = CreateWidget<UPopupConfirmWidget>(this, PopupConfirmWidgetClass);
		if (Popup)
		{
			Popup->SetPopupText(FText::FromString(TEXT("Are you sure?")));
			Popup->OnConfirmClicked.AddDynamic(this, &UPauseMenuWidget::OnQuitConfirmed);
			Popup->AddToViewport();
		}
	}
}

void UPauseMenuWidget::OnQuitConfirmed()
{
	UGameplayStatics::OpenLevel(this, FName("L_Title"));
}