#include "BulletSelectionWidget.h"
#include "Character/PlayerCharacter.h"
#include "UI/Scouting/BulletListItemWidget.h"
#include "Core/OSMKGameState.h"
#include "GameMode/OSMKInGameGameMode.h"
#include "Data/BulletData.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScrollBox.h"
#include "Components/Image.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

void UBulletSelectionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsFlying)
	{
		FlyProgress += InDeltaTime * 3.0f;
		FlyProgress = FMath::Min(FlyProgress, 1.0f);

		if (Img_FlyingBullet)
		{
			FVector2D CurrentPos  = FMath::Lerp(FlyStartPos, FlyEndPos, FlyProgress);
			FVector2D CurrentSize = FMath::Lerp(FlyStartSize, FlyEndSize, FlyProgress);

			FWidgetTransform Transform;
			Transform.Translation = CurrentPos;
			FVector2D BulletImageSize = Img_FlyingBullet->GetCachedGeometry().GetLocalSize();
			if (!BulletImageSize.IsNearlyZero())
			{
				Transform.Scale = CurrentSize / BulletImageSize;
			}
			Img_FlyingBullet->SetRenderTransform(Transform);
		}

		if (FlyProgress >= 1.0f)
		{
			bIsFlying = false;
			if (Img_FlyingBullet)
			{
				Img_FlyingBullet->SetVisibility(ESlateVisibility::Hidden);
			}
			SetInteractable(true);

			if (bIsFlyingToSlot)
			{
				SlotBullets[PendingSlotIndex] = PendingBulletName;
				TargetRotationAngle += 60.0f;
				UpdateSlotImages();
				UpdateListItemCount(PendingBulletName);
				RefreshConfirmButton();
			}
			else
			{
				SlotBullets[PendingSlotIndex] = NAME_None;
				TargetRotationAngle -= 60.0f;
				UpdateSlotImages();
				UpdateListItemCount(PendingBulletName);
				RefreshConfirmButton();
			}
		}
	}

	if (ResetFlyEntries.Num() > 0)
	{
		bool bAllDone = true;

		for (FResetFlyEntry& Entry : ResetFlyEntries)
		{
			Entry.Progress += InDeltaTime * 3.0f;
			Entry.Progress = FMath::Min(Entry.Progress, 1.0f);

			if (Entry.Image)
			{
				FVector2D Pos  = FMath::Lerp(Entry.StartPos,  Entry.EndPos,  Entry.Progress);
				FVector2D Size = FMath::Lerp(Entry.StartSize, Entry.EndSize, Entry.Progress);

				FWidgetTransform T;
				T.Translation = Pos;
				FVector2D ImgSize = Entry.Image->GetCachedGeometry().GetLocalSize();
				if (!ImgSize.IsNearlyZero())
				{
					T.Scale = Size / ImgSize;
				}
				Entry.Image->SetRenderTransform(T);
			}

			if (Entry.Progress < 1.0f)
			{
				bAllDone = false;
			}
		}

		if (bAllDone)
		{
			FinishReset();
		}
	}
	if (!bIsFlying && ResetFlyEntries.Num() == 0 &&	CylinderPanel && !FMath::IsNearlyEqual(CurrentRotationAngle, TargetRotationAngle, 0.1f))
	{
		CurrentRotationAngle = FMath::FInterpTo(CurrentRotationAngle, TargetRotationAngle, InDeltaTime, 10.0f);
		CylinderPanel->SetRenderTransformAngle(CurrentRotationAngle);
	}
}

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
		Btn_Reset->OnHovered.AddDynamic(this, &UBulletSelectionWidget::OnResetHovered);
		Btn_Reset->OnUnhovered.AddDynamic(this, &UBulletSelectionWidget::OnResetUnhovered);
	}
	if (Btn_Pop)
	{
		Btn_Pop->OnClicked.AddDynamic(this, &UBulletSelectionWidget::OnPopClicked);
		Btn_Pop->OnHovered.AddDynamic(this, &UBulletSelectionWidget::OnPopHovered);
		Btn_Pop->OnUnhovered.AddDynamic(this, &UBulletSelectionWidget::OnPopUnhovered);
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
				SlotArray[i]->SetBrushFromTexture(Row->CylinderIcon.LoadSynchronous());
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

void UBulletSelectionWidget::SetInteractable(bool bEnabled)
{
	if (Canvas_FlyOverlay)
	{
		Canvas_FlyOverlay->SetVisibility(bEnabled ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Visible);
	}
}

void UBulletSelectionWidget::StartFlyAnimation(FName BulletName, int32 SlotIndex, bool bToSlot)
{
	UImage* SlotArray[MaxBulletSlots] = { BulletSlot_1, BulletSlot_2, BulletSlot_3, BulletSlot_4, BulletSlot_5, BulletSlot_6 };
	UBulletListItemWidget** ItemPtr = ListItemWidgets.Find(BulletName);
	UImage* SlotImage = (SlotIndex >= 0 && SlotIndex < MaxBulletSlots) ? SlotArray[SlotIndex] : nullptr;

	if (SlotImage && ItemPtr)
	{
		FVector2D SlotPos  = GetCachedGeometry().AbsoluteToLocal(SlotImage->GetCachedGeometry().GetAbsolutePosition());
		FVector2D ItemPos  = GetCachedGeometry().AbsoluteToLocal((*ItemPtr)->GetCachedGeometry().GetAbsolutePosition());
		FVector2D SlotSize = SlotImage->GetCachedGeometry().GetLocalSize();
		FVector2D ItemSize = (*ItemPtr)->GetCachedGeometry().GetLocalSize();

		FlyStartPos  = bToSlot ? ItemPos  : SlotPos;
		FlyEndPos    = bToSlot ? SlotPos  : ItemPos;
		FlyStartSize = bToSlot ? ItemSize : SlotSize;
		FlyEndSize   = bToSlot ? SlotSize : ItemSize;
	}

	PendingBulletName = BulletName;
	PendingSlotIndex  = SlotIndex;
	bIsFlyingToSlot   = bToSlot;
	bIsFlying         = true;
	FlyProgress       = 0.0f;
	SetInteractable(false);

	if (Img_FlyingBullet && BulletDataTable)
	{
		if (FBulletData* Row = BulletDataTable->FindRow<FBulletData>(BulletName, TEXT("")))
		{
			Img_FlyingBullet->SetBrushFromTexture(Row->BulletIcon.LoadSynchronous());
			Img_FlyingBullet->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

void UBulletSelectionWidget::FinishReset()
{
	if (Canvas_FlyOverlay)
	{
		Canvas_FlyOverlay->ClearChildren();
	}
	ResetFlyEntries.Empty();
	ResetFlyImages.Empty();

	for (int32 i = 0; i < MaxBulletSlots; i++)
	{
		SlotBullets[i] = NAME_None;
	}
	TargetRotationAngle = 0.0f;
	UpdateSlotImages();
	for (auto& Pair : ListItemWidgets)
	{
		Pair.Value->SetCount(GetAvailableCount(Pair.Key));
	}
	RefreshConfirmButton();
	SetInteractable(true);
}

void UBulletSelectionWidget::AddBulletToSlot(FName RowName)
{
	if (bIsFlying || ResetFlyEntries.Num() > 0)
	{
		return;
	}
	if (GetSelectedCount(RowName) >= GetAvailableCount(RowName))
	{
		return;
	}

	for (int32 i = 0; i < MaxBulletSlots; i++)
	{
		if (SlotBullets[i].IsNone())
		{
			StartFlyAnimation(RowName, i, true);
			return;
		}
	}
}

void UBulletSelectionWidget::OnPopClicked()
{
	if (bIsFlying || ResetFlyEntries.Num() > 0)
	{
		return;
	}

	OnPopUnhovered();

	for (int32 i = MaxBulletSlots - 1; i >= 0; i--)
	{
		if (!SlotBullets[i].IsNone())
		{
			StartFlyAnimation(SlotBullets[i], i, false);
			return;
		}
	}
}

void UBulletSelectionWidget::OnPopHovered()
{
	if (bIsFlying || ResetFlyEntries.Num() > 0)
	{
		return;
	}

	UImage* SlotArray[MaxBulletSlots] = { BulletSlot_1, BulletSlot_2, BulletSlot_3, BulletSlot_4, BulletSlot_5, BulletSlot_6 };
	for (int32 i = MaxBulletSlots - 1; i >= 0; i--)
	{
		if (!SlotBullets[i].IsNone() && SlotArray[i])
		{
			SlotArray[i]->SetColorAndOpacity(FLinearColor::Red);
			return;
		}
	}
}

void UBulletSelectionWidget::OnPopUnhovered()
{
	UImage* SlotArray[MaxBulletSlots] = { BulletSlot_1, BulletSlot_2, BulletSlot_3, BulletSlot_4, BulletSlot_5, BulletSlot_6 };
	for (int32 i = 0; i < MaxBulletSlots; i++)
	{
		if (SlotArray[i])
		{
			SlotArray[i]->SetColorAndOpacity(FLinearColor::White);
		}
	}
}

void UBulletSelectionWidget::OnResetClicked()
{
	if (bIsFlying || ResetFlyEntries.Num() > 0)
	{
		return;
	}

	OnResetUnhovered();

	if (!Canvas_FlyOverlay || !BulletDataTable)
	{
		FinishReset();
		return;
	}

	UImage* SlotArray[MaxBulletSlots] = { BulletSlot_1, BulletSlot_2, BulletSlot_3, BulletSlot_4, BulletSlot_5, BulletSlot_6 };
	bool bHasAny = false;

	for (int32 i = 0; i < MaxBulletSlots; i++)
	{
		if (SlotBullets[i].IsNone())
		{
			continue;
		}

		FBulletData* Row = BulletDataTable->FindRow<FBulletData>(SlotBullets[i], TEXT(""));
		if (!Row)
		{
			continue;
		}

		UBulletListItemWidget** ItemPtr = ListItemWidgets.Find(SlotBullets[i]);

		UImage* NewImg = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		if (!NewImg)
		{
			continue;
		}

		NewImg->SetBrushFromTexture(Row->BulletIcon.LoadSynchronous());
		UCanvasPanelSlot* CanvasSlot = Canvas_FlyOverlay->AddChildToCanvas(NewImg);
		if (CanvasSlot)
		{
			CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 0.f, 0.f));
			CanvasSlot->SetPosition(FVector2D::ZeroVector);
			CanvasSlot->SetSize(FVector2D(64.f, 64.f));
		}

		FResetFlyEntry Entry;
		Entry.Image    = NewImg;
		Entry.Progress = 0.0f;

		if (SlotArray[i] && ItemPtr)
		{
			Entry.StartPos  = GetCachedGeometry().AbsoluteToLocal(SlotArray[i]->GetCachedGeometry().GetAbsolutePosition());
			Entry.EndPos    = GetCachedGeometry().AbsoluteToLocal((*ItemPtr)->GetCachedGeometry().GetAbsolutePosition());
			Entry.StartSize = SlotArray[i]->GetCachedGeometry().GetLocalSize();
			Entry.EndSize   = (*ItemPtr)->GetCachedGeometry().GetLocalSize();
		}

		ResetFlyEntries.Add(Entry);
		ResetFlyImages.Add(NewImg);
		bHasAny = true;
	}

	if (bHasAny)
	{
		SetInteractable(false);
	}
	else
	{
		FinishReset();
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

void UBulletSelectionWidget::OnResetHovered()
{
	if (bIsFlying || ResetFlyEntries.Num() > 0)
	{
		return;
	}

	UImage* SlotArray[MaxBulletSlots] = { BulletSlot_1, BulletSlot_2, BulletSlot_3, BulletSlot_4, BulletSlot_5, BulletSlot_6 };
	for (int32 i = 0; i < MaxBulletSlots; i++)
	{
		if (!SlotBullets[i].IsNone() && SlotArray[i])
		{
			SlotArray[i]->SetColorAndOpacity(FLinearColor::Red);
		}
	}
}

void UBulletSelectionWidget::OnResetUnhovered()
{
	UImage* SlotArray[MaxBulletSlots] = { BulletSlot_1, BulletSlot_2, BulletSlot_3, BulletSlot_4, BulletSlot_5, BulletSlot_6 };
	for (int32 i = 0; i < MaxBulletSlots; i++)
	{
		if (SlotArray[i])
		{
			SlotArray[i]->SetColorAndOpacity(FLinearColor::White);
		}
	}
}
