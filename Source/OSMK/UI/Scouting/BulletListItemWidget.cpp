#include "UI/Scouting/BulletListItemWidget.h"
#include "BulletTooltipWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UBulletListItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Item)
	{
		Btn_Item->OnClicked.AddDynamic(this, &UBulletListItemWidget::OnItemClicked);
	}
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
			Btn_Item->SetToolTip(TooltipWidget);
		}
	}
}

void UBulletListItemWidget::SetCount(int32 Count)
{
	if (Text_Count)
	{
		Text_Count->SetText(FText::AsNumber(Count));
	}
}

void UBulletListItemWidget::OnItemClicked()
{
	OnBulletItemClicked.ExecuteIfBound(RowName);
}
