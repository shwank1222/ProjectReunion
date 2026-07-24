#include "UI/Scouting/ScoutingWidget.h"
#include "UI/Scouting/BulletSelectionWidget.h"
#include "Components/Button.h"

void UScoutingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BulletSelectionWidget)
	{
		BulletSelectionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Btn_BulletPrep)
	{
		Btn_BulletPrep->OnClicked.AddDynamic(this, &UScoutingWidget::OnBulletPrepClicked);
	}

	if (Btn_Scout)
	{
		Btn_Scout->SetVisibility(ESlateVisibility::Collapsed);
		Btn_Scout->OnClicked.AddDynamic(this, &UScoutingWidget::OnScoutClicked);
	}
}

void UScoutingWidget::OnBulletPrepClicked()
{
	if (BulletSelectionWidget)
	{
		BulletSelectionWidget->SetVisibility(ESlateVisibility::Visible);
	}
	if (Btn_BulletPrep)
	{
		Btn_BulletPrep->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (Btn_Scout)
	{
		Btn_Scout->SetVisibility(ESlateVisibility::Visible);
	}
}

void UScoutingWidget::OnScoutClicked()
{
	if (BulletSelectionWidget)
	{
		BulletSelectionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (Btn_Scout)
	{
		Btn_Scout->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (Btn_BulletPrep)
	{
		Btn_BulletPrep->SetVisibility(ESlateVisibility::Visible);
	}
}
