#include "CreditsWidget.h"
#include "Components/VerticalBox.h"

void UCreditsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;
	SetKeyboardFocus();

	CurrentY = 0.0f;
	CurrentOpacity = 0.0f;
	bIsClosing = false;
	SetRenderOpacity(CurrentOpacity);

	if (Box_Credits)
	{
		Box_Credits->SetRenderTranslation(FVector2D(0.f, CurrentY));
	}
}

void UCreditsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsClosing && CurrentOpacity < 1.0f)
	{
		CurrentOpacity = FMath::Min(1.0f, CurrentOpacity + InDeltaTime * 2.0f);
		SetRenderOpacity(CurrentOpacity);
	}

	if (bIsClosing)
	{
		CurrentOpacity = FMath::Max(0.0f, CurrentOpacity - InDeltaTime * 3.0f);
		SetRenderOpacity(CurrentOpacity);
		if (CurrentOpacity <= 0.0f)
		{
			RemoveFromParent();
		}
		return;
	}

	if (Box_Credits)
	{
		CurrentY -= ScrollSpeed * InDeltaTime;
		Box_Credits->SetRenderTranslation(FVector2D(0.f, CurrentY));

		if (CurrentY <= EndPositionY)
		{
			CloseCredits();
		}
	}
}

FReply UCreditsWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	CloseCredits();
	return FReply::Handled();
}

FReply UCreditsWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	CloseCredits();
	return FReply::Handled();
}

void UCreditsWidget::CloseCredits()
{
	bIsClosing = true;
}