#include "UI/Scouting/BulletListItemWidget.h"

#include "BulletDragDropOperation.h"
#include "BulletDragVisualWidget.h"
#include "BulletTooltipWidget.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UBulletListItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Visible);
}

void UBulletListItemWidget::Init(FName InRowName, UTexture2D* InIcon, const FText& InBulletName, const FText& InBulletDescription)
{
	RowName = InRowName;

	if (Img_Icon && InIcon)
	{
		Img_Icon->SetBrushFromTexture(InIcon);
	}

	if (Text_Name)
	{
		Text_Name->SetText(InBulletName);
	}

	if (TooltipWidgetClass)
	{
		UBulletTooltipWidget* TooltipWidget = CreateWidget<UBulletTooltipWidget>(this, TooltipWidgetClass);
		if (TooltipWidget)
		{
			FText FormattedDescription = FText::FromString(InBulletDescription.ToString().Replace(TEXT("\\n"), TEXT("\n")));
			TooltipWidget->SetTooltipData(InBulletName, FormattedDescription);
			SetToolTip(TooltipWidget);
		}
	}
}

void UBulletListItemWidget::SetCount(int32 Count)
{
	CurrentCount = Count;
	SetCursor(CurrentCount > 0 ? HoverCursor.GetValue() : EMouseCursor::Default);

	if (Text_Count)
	{
		Text_Count->SetText(FText::AsNumber(Count));
	}

	SetIconHidden(false);
}

void UBulletListItemWidget::SetIconHidden(bool bHidden)
{
	if (Img_Icon)
	{
		const bool bHide = bHidden || CurrentCount <= 0;
		Img_Icon->SetRenderOpacity(bHide ? 0.0f : 1.0f);
	}
}

void UBulletListItemWidget::RestoreFromDrag()
{
	SetCount(PreDragCount);
}

void UBulletListItemWidget::OnDragEnded()
{
	SetCursor(CurrentCount > 0 ? HoverCursor.GetValue() : EMouseCursor::Default);
}

void UBulletListItemWidget::SetGrabSettings(float InHeldScale, float InMoveDuration)
{
	HeldBulletScale = InHeldScale;
	GrabMoveDuration = InMoveDuration;
}

void UBulletListItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (CurrentCount <= 0)
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(HoverTimerHandle, [this]()
	{
		OnBulletItemHovered.ExecuteIfBound(RowName);
	}, 2.f, false);
}

void UBulletListItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	GetWorld()->GetTimerManager().ClearTimer(HoverTimerHandle);
	OnBulletItemUnhovered.ExecuteIfBound(RowName);
}

FReply UBulletListItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (CurrentCount > 0 && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	return Reply;
}

void UBulletListItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	UBulletDragDropOperation* DragOperation = Cast<UBulletDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UBulletDragDropOperation::StaticClass()));
	if (!DragOperation)
	{
		return;
	}

	DragOperation->BulletRowName = RowName;
	// DragOperation->Pivot = EDragPivot::CenterLeft;
	DragOperation->Pivot = EDragPivot::MouseDown;
	DragOperation->SourceItem = this;

	if (DragVisualClass)
	{
		UBulletDragVisualWidget* DragVisual = CreateWidget<UBulletDragVisualWidget>(this, DragVisualClass);
		if (DragVisual && Img_Icon)
		{
			DragVisual->InitVisual(Cast<UTexture2D>(Img_Icon->GetBrush().GetResourceObject()));

			const float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
			if (ViewportScale > KINDA_SMALL_NUMBER)
			{
				const FVector2D ItemAbs = InGeometry.GetAbsolutePosition();
				const FVector2D CursorAbs = InMouseEvent.GetScreenSpacePosition();
				const FGeometry& IconGeometry = Img_Icon->GetCachedGeometry();
				const FVector2D IconAbsSize = IconGeometry.GetAbsoluteSize();
				// const FVector2D IconGrabPointAbs = FVector2D(IconGeometry.GetAbsolutePosition()) + FVector2D(0.0f, IconAbsSize.Y * 0.5f);
				// DragVisual->SetGrabStart((IconGrabPointAbs - ItemAbs) / ViewportScale, (CursorAbs - ItemAbs) / ViewportScale, IconAbsSize / ViewportScale, HeldBulletScale, GrabMoveDuration);
				const FVector2D IconTopLeftAbs = FVector2D(IconGeometry.GetAbsolutePosition());
				DragVisual->SetGrabStart((IconTopLeftAbs - ItemAbs) / ViewportScale, (CursorAbs - ItemAbs) / ViewportScale, IconAbsSize / ViewportScale, HeldBulletScale, GrabMoveDuration);
			}
		}
		DragOperation->DefaultDragVisual = DragVisual;
	}

	OutOperation = DragOperation;

	PreDragCount = CurrentCount;
	SetCount(FMath::Max(0, CurrentCount - 1));
	SetCursor(DragCursor.GetValue());

	SetIconHidden(true);
}