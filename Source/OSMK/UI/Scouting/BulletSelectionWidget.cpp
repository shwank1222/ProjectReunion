#include "BulletSelectionWidget.h"
#include "UI/Scouting/BulletListItemWidget.h"
#include "UI/Scouting/BulletSlotWidget.h"
#include "Core/OSMKGameState.h"
#include "GameMode/OSMKInGameGameMode.h"
#include "Data/BulletData.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/HorizontalBox.h"
#include "Engine/DataTable.h"

void UBulletSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SlotBullets.Init(NAME_None, MaxBulletSlots);

	InitSlots();
	PopulateBulletList();
	RefreshConfirmButton();

	if (Btn_Confirm)
	{
		Btn_Confirm->OnClicked.AddDynamic(this, &UBulletSelectionWidget::OnConfirmClicked);
	}
	if (Btn_DebugClose)
    {
           Btn_DebugClose->OnClicked.AddDynamic(this, &UBulletSelectionWidget::OnDebugCloseClicked);
    }
}

void UBulletSelectionWidget::InitSlots()
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
		SlotWidget->OnSlotClicked.BindUObject(this, &UBulletSelectionWidget::RemoveBulletFromSlot);
		Box_Slots->AddChild(SlotWidget);
		SlotWidgets.Add(SlotWidget);
	}
}

void UBulletSelectionWidget::PopulateBulletList()
{
	if (!List_Bullets)
	{
		UE_LOG(LogTemp, Error, TEXT("[BulletSelectionWidget] List_Bullets is null"));
		return;
	}
	if (!BulletDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[BulletSelectionWidget] BulletDataTable is null"));
		return;
	}
	if (!BulletItemWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[BulletSelectionWidget] BulletItemWidgetClass is null"));
		return;
	}

	List_Bullets->ClearChildren();

	TArray<FName> RowNames = BulletDataTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		FBulletData* Row = BulletDataTable->FindRow<FBulletData>(RowName, TEXT(""));
		if (!Row)
		{
			UE_LOG(LogTemp, Warning, TEXT("[BulletSelectionWidget] Row not found: %s"), *RowName.ToString());
			continue;
		}

		UBulletListItemWidget* Item = CreateWidget<UBulletListItemWidget>(this, BulletItemWidgetClass);
		if (!Item)
		{
			UE_LOG(LogTemp, Warning, TEXT("[BulletSelectionWidget] Failed to create item widget for: %s"), *RowName.ToString());
			continue;
		}

		Item->Init(RowName, Row->BulletIcon.LoadSynchronous());
		Item->OnBulletItemClicked.BindUObject(this, &UBulletSelectionWidget::AddBulletToSlot);
		List_Bullets->AddChild(Item);
		UE_LOG(LogTemp, Log, TEXT("[ScoutingWidget] Added bullet item: %s"), *RowName.ToString());
	}
}

void UBulletSelectionWidget::AddBulletToSlot(FName RowName)
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

void UBulletSelectionWidget::RemoveBulletFromSlot(int32 SlotIndex)
{
	if (!SlotBullets.IsValidIndex(SlotIndex))
	{
		return;
	}

	SlotBullets[SlotIndex] = NAME_None;
	SlotWidgets[SlotIndex]->ClearSlot();
	RefreshConfirmButton();
}

void UBulletSelectionWidget::RefreshConfirmButton()
{
	if (!Btn_Confirm)
	{
		return;
	}

	bool bAllFilled = !SlotBullets.Contains(NAME_None);
	Btn_Confirm->SetIsEnabled(bAllFilled);
}

void UBulletSelectionWidget::OnConfirmClicked()
{
	if (AOSMKGameState* GS = GetWorld()->GetGameState<AOSMKGameState>())
	{
		GS->EndScoutingPhase();
	}

	if (AOSMKInGameGameMode* GM = Cast<AOSMKInGameGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->PossessPlayerCharacter();
	}
}

void UBulletSelectionWidget::OnDebugCloseClicked()
{
	if (AOSMKGameState* GS = GetWorld()->GetGameState<AOSMKGameState>())
	{
		GS->EndScoutingPhase();
	}

	RemoveFromParent();
}