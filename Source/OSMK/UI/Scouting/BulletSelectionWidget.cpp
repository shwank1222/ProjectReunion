#include "BulletSelectionWidget.h"
#include "Character/PlayerCharacter.h"
#include "UI/Scouting/BulletListItemWidget.h"
#include "Core/OSMKGameState.h"
#include "GameMode/OSMKInGameGameMode.h"
#include "Data/BulletData.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/Image.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

int32 UBulletSelectionWidget::GetAvailableCount(FName RowName) const
{
	if (AOSMKInGameGameMode* GM = Cast<AOSMKInGameGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (const TMap<FName, int32>* Counts = GM->GetCurrentStageBulletCounts())
		{
			const int32* Found = Counts->Find(RowName);
			return Found ? *Found : 0;
		}
	}
	return 0;
}

int32 UBulletSelectionWidget::GetSelectedCount(FName RowName) const
{
	int32 Count = 0;
	for (const FName& Name : SlotBullets)
	{
		if (Name == RowName)
		{
			Count++;
		}
	}
	return Count;
}

void UBulletSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SlotBullets.Init(NAME_None, MaxBulletSlots);

	UpdateSlotImages();
	PopulateBulletList();
	RefreshConfirmButton();

	if (Btn_Confirm)
	{
		Btn_Confirm->OnClicked.AddDynamic(this, &UBulletSelectionWidget::OnConfirmClicked);
	}
	if (Btn_Reset)
	{
		Btn_Reset->OnClicked.AddDynamic(this, &UBulletSelectionWidget::OnResetClicked);
	}
	if (Btn_Pop)
	{
		Btn_Pop->OnClicked.AddDynamic(this, &UBulletSelectionWidget::OnPopClicked);
	}
}

void UBulletSelectionWidget::UpdateSlotImages()
{
	UImage* SlotArray[MaxBulletSlots] = { BulletSlot_1, BulletSlot_2, BulletSlot_3, BulletSlot_4, BulletSlot_5, BulletSlot_6 };
	
	for (int32 i = 0; i < MaxBulletSlots; i++)
	{
		if (!SlotArray[i])
		{
			continue;
		}

		if (!SlotBullets[i].IsNone())
		{
			if (FBulletData* Row = BulletDataTable->FindRow<FBulletData>(SlotBullets[i], TEXT("")))
			{
				SlotArray[i]->SetBrushFromTexture(Row->BulletIcon.LoadSynchronous());
			}
		}
		else
		{
			SlotArray[i]->SetBrushFromTexture(nullptr);
		}
	}
}

void UBulletSelectionWidget::PopulateBulletList()
{
	if (!List_Bullets || !BulletDataTable || !BulletItemWidgetClass)
	{
		return;
	}

	List_Bullets->ClearChildren();
	ListItemWidgets.Empty();

	for (const FName& RowName : BulletDataTable->GetRowNames())
	{
		const int32 Available = GetAvailableCount(RowName);
		if (Available <= 0)
		{
			continue;
		}

		FBulletData* Row = BulletDataTable->FindRow<FBulletData>(RowName, TEXT(""));
		if (!Row)
		{
			continue;
		}

		UBulletListItemWidget* Item = CreateWidget<UBulletListItemWidget>(this, BulletItemWidgetClass);
		if (!Item)
		{
			continue;
		}

		Item->Init(RowName, Row->BulletIcon.LoadSynchronous(), Row->BulletName, Row->BulletDescription);
		Item->SetCount(Available);
		Item->OnBulletItemClicked.BindUObject(this, &UBulletSelectionWidget::AddBulletToSlot);
		List_Bullets->AddChild(Item);
		ListItemWidgets.Add(RowName, Item);
	}
}

void UBulletSelectionWidget::UpdateListItemCount(FName RowName)
{
	if (UBulletListItemWidget** ItemPtr = ListItemWidgets.Find(RowName))
	{
		const int32 Remaining = GetAvailableCount(RowName) - GetSelectedCount(RowName);
		(*ItemPtr)->SetCount(FMath::Max(0, Remaining));
	}
}

void UBulletSelectionWidget::AddBulletToSlot(FName RowName)
{
	if (GetSelectedCount(RowName) >= GetAvailableCount(RowName))
	{
		return;
	}

	for (int32 i = 0; i < MaxBulletSlots; i++)
	{
		if (SlotBullets[i].IsNone())
		{
			SlotBullets[i] = RowName;
			UpdateSlotImages();
			UpdateListItemCount(RowName);
			RefreshConfirmButton();
			return;
		}
	}
}
void UBulletSelectionWidget::OnPopClicked()
{
	for (int32 i = MaxBulletSlots - 1; i >= 0; i--)
	{
		if (!SlotBullets[i].IsNone())
		{
			const FName RemovedBullet = SlotBullets[i];
			SlotBullets[i] = NAME_None;
			UpdateSlotImages();
			UpdateListItemCount(RemovedBullet);
			RefreshConfirmButton();
			return;
		}
	}
}

void UBulletSelectionWidget::RefreshConfirmButton()
{
	if (!Btn_Confirm)
	{
		return;
	}

	Btn_Confirm->SetIsEnabled(!SlotBullets.Contains(NAME_None));
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
		GM->ActivateEnemies();
	}

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		for (const FName& BulletName : SlotBullets)
		{
			if (!BulletName.IsNone())
			{
				Player->AddAmmo(BulletName);
			}
		}
	}

	if (UUserWidget* ScoutingWidget = Cast<UUserWidget>(GetOuter()))
	{
		ScoutingWidget->RemoveFromParent();
	}
	else if (UUserWidget* ParentWidget = Cast<UUserWidget>(GetParent()))
	{
		ParentWidget->RemoveFromParent();
	}
	else
	{
		RemoveFromParent();
	}
}

void UBulletSelectionWidget::OnResetClicked()
{
	for (int32 i = 0; i < MaxBulletSlots; i++)
	{
		SlotBullets[i] = NAME_None;
	}
	UpdateSlotImages();

	for (auto& Pair : ListItemWidgets)
	{
		Pair.Value->SetCount(GetAvailableCount(Pair.Key));
	}
	RefreshConfirmButton();
}