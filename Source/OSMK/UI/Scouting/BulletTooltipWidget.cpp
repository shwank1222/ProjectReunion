#include "BulletTooltipWidget.h"
#include "Components/TextBlock.h"

void UBulletTooltipWidget::SetTooltipData(const FText& InBulletName, const FText& InBulletDescription)
{
	if (Text_BulletName)
	{
		Text_BulletName->SetText(InBulletName);
	}

	if (Text_BulletDescription)
	{
		Text_BulletDescription->SetText(InBulletDescription);
	}
}