#include "BulletCountdownWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Character/PlayerCharacter.h"
#include "GameFramework/PlayerController.h"

void UBulletCountdownWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->OnPossessedPawnChanged.AddDynamic(this, &UBulletCountdownWidget::OnPawnChanged);
	}

	TryBindToPlayerCharacter();
}

void UBulletCountdownWidget::NativeDestruct()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->OnPossessedPawnChanged.RemoveDynamic(this, &UBulletCountdownWidget::OnPawnChanged);
	}

	if (CachedPlayerCharacter.IsValid())
	{
		CachedPlayerCharacter->OnLoadedAmmoChanged.RemoveDynamic(this, &UBulletCountdownWidget::HandleAmmoChanged);
	}

	Super::NativeDestruct();
}

void UBulletCountdownWidget::OnPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (CachedPlayerCharacter.IsValid())
	{
		CachedPlayerCharacter->OnLoadedAmmoChanged.RemoveDynamic(this, &UBulletCountdownWidget::HandleAmmoChanged);
		CachedPlayerCharacter = nullptr;
	}

	PreviousAmmoCount = -1;
	TryBindToPlayerCharacter();
}

void UBulletCountdownWidget::TryBindToPlayerCharacter()
{
	APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if (!PlayerChar)
	{
		return;
	}

	CachedPlayerCharacter = PlayerChar;
	CachedPlayerCharacter->OnLoadedAmmoChanged.AddDynamic(this, &UBulletCountdownWidget::HandleAmmoChanged);
	PreviousAmmoCount = CachedPlayerCharacter->NumOfLoadedAmmo();
}

void UBulletCountdownWidget::HandleAmmoChanged()
{
	if (!CachedPlayerCharacter.IsValid())
	{
		return;
	}

	const int32 CurrentAmmoCount = CachedPlayerCharacter->NumOfLoadedAmmo();
	if (CurrentAmmoCount >= PreviousAmmoCount)
	{
		PreviousAmmoCount = CurrentAmmoCount;
		return;
	}

	PreviousAmmoCount = CurrentAmmoCount;

	if (Text_Countdown)
	{
		Text_Countdown->SetText(FText::AsNumber(FMath::Clamp(CurrentAmmoCount, 0, 5)));
	}

	if (Anim_ZoomFade)
	{
		PlayAnimation(Anim_ZoomFade);
	}
}
