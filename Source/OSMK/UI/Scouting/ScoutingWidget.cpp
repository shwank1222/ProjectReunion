#include "UI/Scouting/ScoutingWidget.h"
#include "UI/Scouting/BulletSelectionWidget.h"
#include "Components/Button.h"
#include "Core/OSMKSlowMotionSubsystem.h"

void UScoutingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	if (BulletSelectionWidget)
	{
		BulletSelectionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Btn_BulletPrep)
	{
		Btn_BulletPrep->OnClicked.AddDynamic(this, &UScoutingWidget::OnBulletPrepClicked);
		Btn_BulletPrep->SetIsEnabled(true);
	}

	if (Btn_Scout)
	{
		Btn_Scout->OnClicked.AddDynamic(this, &UScoutingWidget::OnScoutClicked);
		Btn_Scout->SetIsEnabled(false);
	}

	SetGimmickHighlight(true);
}

void UScoutingWidget::OnBulletPrepClicked()
{
	if (BulletSelectionWidget)
	{
		BulletSelectionWidget->SetVisibility(ESlateVisibility::Visible);
	}
	if (Btn_BulletPrep)
	{
		Btn_BulletPrep->SetIsEnabled(false);
	}
	if (Btn_Scout)
	{
		Btn_Scout->SetIsEnabled(true);
	}
	SetGimmickHighlight(false);
	SetUserFocus(GetOwningPlayer());
}

void UScoutingWidget::OnScoutClicked()
{
	if (BulletSelectionWidget)
	{
		BulletSelectionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (Btn_Scout)
	{
		Btn_Scout->SetIsEnabled(false);
	}
	if (Btn_BulletPrep)
	{
		Btn_BulletPrep->SetIsEnabled(true);
	}
	SetGimmickHighlight(true);
	SetUserFocus(GetOwningPlayer());
}

void UScoutingWidget::SetGimmickHighlight(bool bEnable) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (UOSMKSlowMotionSubsystem* SlowMotion = World->GetSubsystem<UOSMKSlowMotionSubsystem>())
	{
		if (bEnable)
		{
			SlowMotion->ApplyGimmickHighlight();
		}
		else
		{
			SlowMotion->RestoreGimmickHighlight();
		}
	}
}
