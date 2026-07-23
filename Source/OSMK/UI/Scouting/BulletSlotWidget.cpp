#include "UI/Scouting/BulletSlotWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UBulletSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Slot)
	{
		Btn_Slot->OnClicked.AddDynamic(this, &UBulletSlotWidget::OnSlotButtonClicked);
	}
}

void UBulletSlotWidget::Init(int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
	ClearSlot();
}

void UBulletSlotWidget::SetBullet(FName InRowName, UTexture2D* InIcon)
{
	CurrentRowName = InRowName;

	if (Img_Icon && InIcon)
	{
		Img_Icon->SetBrushFromTexture(InIcon);
	}
}

void UBulletSlotWidget::ClearSlot()
{
	CurrentRowName = NAME_None;

	if (Img_Icon)
	{
		Img_Icon->SetBrushFromTexture(nullptr);
	}
}

void UBulletSlotWidget::OnSlotButtonClicked()
{
	if (!IsEmpty())
	{
		OnSlotClicked.ExecuteIfBound(SlotIndex);
	}
}
