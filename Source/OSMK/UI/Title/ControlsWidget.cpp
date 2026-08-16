#include "UI/Title/ControlsWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"

void UControlsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	if (Btn_InGame)
	{
		Btn_InGame->OnClicked.AddDynamic(this, &UControlsWidget::OnInGameClicked);
	}
	if (Btn_Scout)
	{
		Btn_Scout->OnClicked.AddDynamic(this, &UControlsWidget::OnScoutClicked);
	}

	if (CP_InGame)
	{
		CP_InGame->SetVisibility(ESlateVisibility::Visible);
	}
	if (CP_Scout)
	{
		CP_Scout->SetVisibility(ESlateVisibility::Hidden);
	}
}

FReply UControlsWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		RemoveFromParent();
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UControlsWidget::OnInGameClicked()
{
	if (CP_InGame)
	{
		CP_InGame->SetVisibility(ESlateVisibility::Visible);
	}
	if (CP_Scout)
	{
		CP_Scout->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UControlsWidget::OnScoutClicked()
{
	if (CP_Scout)
	{
		CP_Scout->SetVisibility(ESlateVisibility::Visible);
	}
	if (CP_InGame)
	{
		CP_InGame->SetVisibility(ESlateVisibility::Hidden);
	}
}
