#include "PopupConfirmWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UPopupConfirmWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Confirm)
	{
		Btn_Confirm->OnClicked.AddDynamic(this, &UPopupConfirmWidget::HandleConfirmClicked);
	}
	if (Btn_Cancel)
	{
		Btn_Cancel->OnClicked.AddDynamic(this, &UPopupConfirmWidget::HandleCancelClicked);
	}
}

void UPopupConfirmWidget::SetPopupText(const FText& InMessage)
{
	if (Text_Message)
	{
		Text_Message->SetText(InMessage);
	}
}

void UPopupConfirmWidget::HandleConfirmClicked()
{
	OnConfirmClicked.Broadcast();
	RemoveFromParent();
}

void UPopupConfirmWidget::HandleCancelClicked()
{
	OnCancelClicked.Broadcast();
	RemoveFromParent();
}