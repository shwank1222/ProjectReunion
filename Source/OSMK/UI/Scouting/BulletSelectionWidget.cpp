#include "BulletSelectionWidget.h"
#include "BulletDragDropOperation.h"
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
#include "Components/TextBlock.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

void UBulletSelectionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsFlying)
	{
		FlyProgress += InDeltaTime * FlySpeed;
		FlyProgress = FMath::Min(FlyProgress, 1.0f);

		if (Img_FlyingBullet)
		{
			FVector2D CurrentPos = FMath::Lerp(FlyStartPos, FlyEndPos, FlyProgress);

			FWidgetTransform Transform;
			Transform.Translation = CurrentPos;
			Transform.Angle = FlyAngle;

			const FVector2D BoxSize = Img_FlyingBullet->GetCachedGeometry().GetLocalSize();
			if (BoxSize.X > KINDA_SMALL_NUMBER && BoxSize.Y > KINDA_SMALL_NUMBER &&
				FlyTextureSize.X > KINDA_SMALL_NUMBER && FlyTextureSize.Y > KINDA_SMALL_NUMBER)
			{
				const FVector2D TargetBox = FMath::Lerp(FlyStartSize, FlyEndSize, FlyProgress);
				const float Fit = FMath::Min(TargetBox.X / FlyTextureSize.X, TargetBox.Y / FlyTextureSize.Y);
				const FVector2D DesiredSize = FlyTextureSize * Fit;
				Transform.Scale = DesiredSize / BoxSize;
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

			switch (FlyMode)
			{
			case EBulletFlyMode::LoadToCylinder:
				SlotBullets[PendingSlotIndex] = PendingBulletName;
				TargetRotationAngle += 60.0f;
				UpdateSlotImages();
				UpdateListItemCount(PendingBulletName);
				SetItemIconHidden(PendingBulletName, false);
				RefreshConfirmButton();
				break;

			case EBulletFlyMode::PopFromCylinder:
				SlotBullets[PendingSlotIndex] = NAME_None;
				TargetRotationAngle -= 60.0f;
				UpdateSlotImages();
				UpdateListItemCount(PendingBulletName);
				RefreshConfirmButton();
				break;

			case EBulletFlyMode::ReturnToItem:
				UpdateListItemCount(PendingBulletName);
				SetItemIconHidden(PendingBulletName, false);
				RefreshConfirmButton();
				break;
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

	if (Btn_Confirm)
	{
		Btn_Confirm->SetVisibility(ESlateVisibility::Hidden);
		Btn_Confirm->OnClicked.AddDynamic(this, &UBulletSelectionWidget::OnConfirmClicked);
	}

	RefreshConfirmButton();
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
				SlotArray[i]->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
			}
		}
		else
		{
			SlotArray[i]->SetBrushFromTexture(nullptr);
			SlotArray[i]->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));
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

FVector2D UBulletSelectionWidget::LocalPositionOf(const UWidget* Widget) const
{
	if (!Widget)
	{
		return FVector2D::ZeroVector;
	}
	return GetCachedGeometry().AbsoluteToLocal(Widget->GetCachedGeometry().GetAbsolutePosition());
}

void UBulletSelectionWidget::StartCursorFly(FName BulletName, const FVector2D& CursorScreenPos, int32 SlotIndex, EBulletFlyMode Mode)
{
	UImage* SlotArray[MaxBulletSlots] = { BulletSlot_1, BulletSlot_2, BulletSlot_3, BulletSlot_4, BulletSlot_5, BulletSlot_6 };
	UBulletListItemWidget** ItemPtr = ListItemWidgets.Find(BulletName);
	UImage* SlotImage = (SlotIndex >= 0 && SlotIndex < MaxBulletSlots) ? SlotArray[SlotIndex] : nullptr;

	const FVector2D ItemSize = ItemPtr ? (*ItemPtr)->GetCachedGeometry().GetLocalSize() : FVector2D::ZeroVector;
	const FVector2D SlotSize = SlotImage ? SlotImage->GetCachedGeometry().GetLocalSize() : ItemSize;

	FlyStartSize = ItemSize;
	const FVector2D CursorLocal = GetCachedGeometry().AbsoluteToLocal(CursorScreenPos);
	FlyStartPos  = CursorLocal - FlyStartSize * 0.5f;

	if (Mode == EBulletFlyMode::LoadToCylinder && SlotImage)
	{
		FlyEndPos  = LocalPositionOf(SlotImage);
		FlyEndSize = SlotSize;
	}
	else if (ItemPtr)
	{
		FlyEndPos  = LocalPositionOf(*ItemPtr);
		FlyEndSize = ItemSize;
	}

	PendingBulletName = BulletName;
	PendingSlotIndex  = SlotIndex;
	FlyMode = Mode;
	FlyAngle = FlyingBulletUprightAngle;
	bIsFlying = true;
	FlyProgress = 0.0f;
	SetInteractable(false);

	if (Img_FlyingBullet && BulletDataTable)
	{
		if (FBulletData* Row = BulletDataTable->FindRow<FBulletData>(BulletName, TEXT("")))
		{
			UTexture2D* Icon = Row->BulletIcon.LoadSynchronous();
			Img_FlyingBullet->SetBrushFromTexture(Icon);
			Img_FlyingBullet->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
			Img_FlyingBullet->SetVisibility(ESlateVisibility::HitTestInvisible);
			FlyTextureSize = Icon ? FVector2D(Icon->GetSizeX(), Icon->GetSizeY()) : FVector2D::ZeroVector;
		}
	}
}

void UBulletSelectionWidget::StartPopFly(int32 SlotIndex)
{
	UImage* SlotArray[MaxBulletSlots] = { BulletSlot_1, BulletSlot_2, BulletSlot_3, BulletSlot_4, BulletSlot_5, BulletSlot_6 };
	if (SlotIndex < 0 || SlotIndex >= MaxBulletSlots)
	{
		return;
	}

	const FName BulletName = SlotBullets[SlotIndex];
	UImage* SlotImage = SlotArray[SlotIndex];
	UBulletListItemWidget** ItemPtr = ListItemWidgets.Find(BulletName);

	if (SlotImage && ItemPtr)
	{
		FlyStartPos  = LocalPositionOf(SlotImage);
		FlyEndPos    = LocalPositionOf(*ItemPtr);
		FlyStartSize = SlotImage->GetCachedGeometry().GetLocalSize();
		FlyEndSize   = (*ItemPtr)->GetCachedGeometry().GetLocalSize();
	}

	PendingBulletName = BulletName;
	PendingSlotIndex  = SlotIndex;
	FlyMode           = EBulletFlyMode::PopFromCylinder;
	FlyAngle          = 0.0f;
	bIsFlying         = true;
	FlyProgress       = 0.0f;
	SetInteractable(false);

	if (Img_FlyingBullet && BulletDataTable)
	{
		if (FBulletData* Row = BulletDataTable->FindRow<FBulletData>(BulletName, TEXT("")))
		{
			UTexture2D* Icon = Row->BulletIcon.LoadSynchronous();
			Img_FlyingBullet->SetBrushFromTexture(Icon);
			Img_FlyingBullet->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
			Img_FlyingBullet->SetVisibility(ESlateVisibility::HitTestInvisible);
			FlyTextureSize = Icon ? FVector2D(Icon->GetSizeX(), Icon->GetSizeY()) : FVector2D::ZeroVector;
		}
	}
}

void UBulletSelectionWidget::SetItemIconHidden(FName RowName, bool bHidden)
{
	if (UBulletListItemWidget** ItemPtr = ListItemWidgets.Find(RowName))
	{
		(*ItemPtr)->SetIconHidden(bHidden);
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

void UBulletSelectionWidget::HandleBulletDropped(FName RowName, const FVector2D& CursorScreenPos, bool bOnCylinder)
{
	if (bIsFlying || ResetFlyEntries.Num() > 0)
	{
		UpdateListItemCount(RowName);
		SetItemIconHidden(RowName, false);
		return;
	}

	if (bOnCylinder && GetSelectedCount(RowName) < GetAvailableCount(RowName))
	{
		for (int32 i = 0; i < MaxBulletSlots; i++)
		{
			if (SlotBullets[i].IsNone())
			{
				StartCursorFly(RowName, CursorScreenPos, i, EBulletFlyMode::LoadToCylinder);
				return;
			}
		}
	}

	StartCursorFly(RowName, CursorScreenPos, INDEX_NONE, EBulletFlyMode::ReturnToItem);
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
			StartPopFly(i);
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
			SlotArray[i]->SetColorAndOpacity(SlotBullets[i].IsNone() ? FLinearColor(1.f, 1.f, 1.f, 0.f) : FLinearColor::White);
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

	const bool bAllFilled = !SlotBullets.Contains(NAME_None);

	if (bAllFilled)
	{
		Btn_Confirm->SetVisibility(ESlateVisibility::Visible);
		if (Img_Action)
		{
			Img_Action->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		if (Txt_Action)
		{
			Txt_Action->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		if (Anim_Confirm)
		{
			PlayAnimation(Anim_Confirm, 0.f, 0);
		}
	}
	else
	{
		Btn_Confirm->SetVisibility(ESlateVisibility::Hidden);
		if (Img_Action)
		{
			Img_Action->SetVisibility(ESlateVisibility::Hidden);
		}
		if (Txt_Action)
		{
			Txt_Action->SetVisibility(ESlateVisibility::Hidden);
		}
		if (Anim_Confirm)
		{
			StopAnimation(Anim_Confirm);
		}
	}
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
			SlotArray[i]->SetColorAndOpacity(SlotBullets[i].IsNone() ? FLinearColor(1.f, 1.f, 1.f, 0.f) : FLinearColor::White);
		}
	}
}

bool UBulletSelectionWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	
	if (UBulletDragDropOperation* BulletOp = Cast<UBulletDragDropOperation>(InOperation))
	{
		const FVector2D CursorScreenPos = InDragDropEvent.GetScreenSpacePosition();
		const bool bOnCylinder = CylinderPanel && CylinderPanel->GetCachedGeometry().IsUnderLocation(CursorScreenPos);
		HandleBulletDropped(BulletOp->BulletRowName, CursorScreenPos, bOnCylinder);
		if (BulletOp->SourceItem.IsValid())
		{
			BulletOp->SourceItem->OnDragEnded();
		}
		return true;
	}
	return false;
}

void UBulletSelectionWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	if (UBulletDragDropOperation* BulletOp = Cast<UBulletDragDropOperation>(InOperation))
	{
		HandleBulletDropped(BulletOp->BulletRowName, InDragDropEvent.GetScreenSpacePosition(), false);
	}
}