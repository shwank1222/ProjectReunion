#include "IngameBulletSlotWidget.h"
#include "Components/Image.h"
#include "Character/PlayerCharacter.h"
#include "Components/CanvasPanel.h"
#include "Data/BulletData.h"
#include "GameFramework/PlayerController.h"

void UIngameBulletSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->OnPossessedPawnChanged.AddDynamic(this, &UIngameBulletSlotWidget::OnPawnChanged);
	}

	TryBindToPlayerCharacter();
}

void UIngameBulletSlotWidget::NativeDestruct()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->OnPossessedPawnChanged.RemoveDynamic(this, &UIngameBulletSlotWidget::OnPawnChanged);
	}

	if (CachedPlayerCharacter.IsValid())
	{
		CachedPlayerCharacter->OnLoadedAmmoChanged.RemoveDynamic(this, &UIngameBulletSlotWidget::UpdateBullets);
	}

	Super::NativeDestruct();
}

void UIngameBulletSlotWidget::OnPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (CachedPlayerCharacter.IsValid())
	{
		CachedPlayerCharacter->OnLoadedAmmoChanged.RemoveDynamic(this, &UIngameBulletSlotWidget::UpdateBullets);
		CachedPlayerCharacter = nullptr;
	}

	TryBindToPlayerCharacter();
}

void UIngameBulletSlotWidget::TryBindToPlayerCharacter()
{
	APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if (!PlayerChar)
	{
		return;
	}

	CachedPlayerCharacter = PlayerChar;
	CachedPlayerCharacter->OnLoadedAmmoChanged.AddDynamic(this, &UIngameBulletSlotWidget::UpdateBullets);
	UpdateBullets();
}

void UIngameBulletSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CylinderPanel && !FMath::IsNearlyEqual(CurrentRotationAngle, TargetRotationAngle, 0.1f))
	{
		CurrentRotationAngle = FMath::FInterpTo(CurrentRotationAngle, TargetRotationAngle, InDeltaTime, 10.0f);
		CylinderPanel->SetRenderTransformAngle(CurrentRotationAngle);
	}
}

void UIngameBulletSlotWidget::UpdateBullets()
{
	if (!CachedPlayerCharacter.IsValid())
	{
		CachedPlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
		if (!CachedPlayerCharacter.IsValid())
		{
			return;
		}
	}

	const TArray<FBulletData> LoadedAmmo = CachedPlayerCharacter->GetLoadedAmmo();
	
	if (PreviousAmmoCount > 0 && LoadedAmmo.Num() < PreviousAmmoCount)
	{
		TargetRotationAngle += 60.0f;
	}
	PreviousAmmoCount = LoadedAmmo.Num();

	const int32 MaxSlots = 6;

	UImage* SlotArray[MaxSlots] = { BulletSlot_1, BulletSlot_2, BulletSlot_3, BulletSlot_4, BulletSlot_5, BulletSlot_6 };

	for (int32 i = 0; i < MaxSlots; i++)
	{
		if (!SlotArray[i])
		{
			continue;
		}

		int32 TargetIndex = MaxSlots - 1 - i;

		if (LoadedAmmo.IsValidIndex(TargetIndex))
		{
			UTexture2D* Icon = LoadedAmmo[TargetIndex].CylinderIcon.LoadSynchronous();
			if (Icon)
			{
				SlotArray[i]->SetBrushFromTexture(Icon);
				SlotArray[i]->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				SlotArray[i]->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
		{
			SlotArray[i]->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
