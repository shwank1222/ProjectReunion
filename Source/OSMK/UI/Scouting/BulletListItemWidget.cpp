#include "UI/Scouting/BulletListItemWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UBulletListItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Item)
	{
		Btn_Item->OnClicked.AddDynamic(this, &UBulletListItemWidget::OnItemClicked);
	}
}

void UBulletListItemWidget::Init(FName InRowName, UTexture2D* InIcon)
{
	RowName = InRowName;

	if (Img_Icon && InIcon)
	{
		Img_Icon->SetBrushFromTexture(InIcon);
	}
}

void UBulletListItemWidget::OnItemClicked()
{
	OnBulletItemClicked.ExecuteIfBound(RowName);
}
