#include "UI/Title/TitleWidget.h"

#include "PopupConfirmWidget.h"
#include "Components/Button.h"
#include "Data/StageData.h"
#include "Kismet/GameplayStatics.h"

void UTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}
	
	if (Btn_GameStart)
	{
		Btn_GameStart->OnClicked.AddDynamic(this, &UTitleWidget::OnClickGameStart);
	}
	if (Btn_Settings)
	{
		Btn_Settings->OnClicked.AddDynamic(this, &UTitleWidget::OnClickSettings);
	}
	if (Btn_Credits)
	{
		Btn_Credits->OnClicked.AddDynamic(this, &UTitleWidget::OnClickCredits);
	}
	if (Btn_Quit)
	{
		Btn_Quit->OnClicked.AddDynamic(this, &UTitleWidget::OnClickQuit);
	}
}

void UTitleWidget::OnClickGameStart()
{
	if (StageDataAsset)
	{
		if (!StageDataAsset->InGameLevel.IsNull())
		{
			UGameplayStatics::OpenLevelBySoftObjectPtr(this, StageDataAsset->InGameLevel);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[StageData] Null StageLevel"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[TitleWidget] Null StageData"));
	}
}

void UTitleWidget::OnClickSettings()
{
}

void UTitleWidget::OnClickCredits()
{
	if (CreditsWidgetClass)
	{
		UUserWidget* CreditsWidget = CreateWidget<UUserWidget>(this, CreditsWidgetClass);
		if (CreditsWidget)
		{
			CreditsWidget->AddToViewport();
		}
	}
}

void UTitleWidget::OnClickQuit()
{
	if (PopupConfirmWidgetClass)
	{
		UPopupConfirmWidget* Popup = CreateWidget<UPopupConfirmWidget>(this, PopupConfirmWidgetClass);
		if (Popup)
		{
			Popup->SetPopupText(FText::FromString(TEXT("Are you sure?")));
			Popup->OnConfirmClicked.AddDynamic(this, &UTitleWidget::OnQuitConfirmed);
			Popup->AddToViewport();
		}
	}
}

void UTitleWidget::OnQuitConfirmed()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}