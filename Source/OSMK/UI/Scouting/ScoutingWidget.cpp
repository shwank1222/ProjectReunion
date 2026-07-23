#include "UI/Scouting/ScoutingWidget.h"
#include "UI/Scouting/BulletListItemWidget.h"
#include "UI/Scouting/BulletSlotWidget.h"
#include "Core/OSMKGameState.h"
#include "Data/BulletData.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/HorizontalBox.h"
#include "Engine/DataTable.h"

void UScoutingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SlotBullets.Init(NAME_None, MaxBulletSlots);

	InitSlots();
	PopulateBulletList();
	RefreshConfirmButton();

	if (Btn_Confirm)
	{
		Btn_Confirm->OnClicked.AddDynamic(this, &UScoutingWidget::OnConfirmClicked);
	}
}

void UScoutingWidget::InitSlots()
{
	if (!Box_Slots || !SlotWidgetClass)
	{
		return;
	}

	Box_Slots->ClearChildren();
	SlotWidgets.Empty();

	for (int32 i = 0; i < MaxBulletSlots; i++)
	{
		UBulletSlotWidget* SlotWidget = CreateWidget<UBulletSlotWidget>(this, SlotWidgetClass);
		if (!SlotWidget)
		{
			continue;
		}

		SlotWidget->Init(i);
		SlotWidget->OnSlotClicked.BindUObject(this, &UScoutingWidget::RemoveBulletFromSlot);
		Box_Slots->AddChild(SlotWidget);
		SlotWidgets.Add(SlotWidget);
	}
}

void UScoutingWidget::PopulateBulletList()
{
	if (!List_Bullets)
	{
		UE_LOG(LogTemp, Error, TEXT("[ScoutingWidget] List_Bullets is null"));
		return;
	}
	if (!BulletDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[ScoutingWidget] BulletDataTable is null"));
		return;
	}
	if (!BulletItemWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[ScoutingWidget] BulletItemWidgetClass is null"));
		return;
	}

	List_Bullets->ClearChildren();

	TArray<FName> RowNames = BulletDataTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		FBulletData* Row = BulletDataTable->FindRow<FBulletData>(RowName, TEXT(""));
		if (!Row)
		{
			UE_LOG(LogTemp, Warning, TEXT("[ScoutingWidget] Row not found: %s"), *RowName.ToString());
			continue;
		}

		UBulletListItemWidget* Item = CreateWidget<UBulletListItemWidget>(this, BulletItemWidgetClass);
		if (!Item)
		{
			UE_LOG(LogTemp, Warning, TEXT("[ScoutingWidget] Failed to create item widget for: %s"), *RowName.ToString());
			continue;
		}

		Item->Init(RowName, Row->BulletIcon.LoadSynchronous());
		Item->OnBulletItemClicked.BindUObject(this, &UScoutingWidget::AddBulletToSlot);
		List_Bullets->AddChild(Item);
		UE_LOG(LogTemp, Log, TEXT("[ScoutingWidget] Added bullet item: %s"), *RowName.ToString());
	}
}

void UScoutingWidget::AddBulletToSlot(FName RowName)
{
	for (int32 i = 0; i < MaxBulletSlots; i++)
	{
		if (SlotBullets[i].IsNone())
		{
			FBulletData* Row = BulletDataTable->FindRow<FBulletData>(RowName, TEXT(""));
			if (!Row)
			{
				return;
			}

			SlotBullets[i] = RowName;
			SlotWidgets[i]->SetBullet(RowName, Row->BulletIcon.LoadSynchronous());
			RefreshConfirmButton();
			return;
		}
	}
}

void UScoutingWidget::RemoveBulletFromSlot(int32 SlotIndex)
{
	if (!SlotBullets.IsValidIndex(SlotIndex))
	{
		return;
	}

	SlotBullets[SlotIndex] = NAME_None;
	SlotWidgets[SlotIndex]->ClearSlot();
	RefreshConfirmButton();
}

void UScoutingWidget::RefreshConfirmButton()
{
	if (!Btn_Confirm)
	{
		return;
	}

	bool bAllFilled = !SlotBullets.Contains(NAME_None);
	Btn_Confirm->SetIsEnabled(bAllFilled);
}

void UScoutingWidget::OnConfirmClicked()
{
	if (AOSMKGameState* GS = GetWorld()->GetGameState<AOSMKGameState>())
	{
		GS->EndScoutingPhase();
	}
}
