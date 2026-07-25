#include "OSMKIngameHUD.h"

#include "BulletCountdownWidget.h"
#include "IngameBulletSlotWidget.h"
#include "Character/PlayerCharacter.h"
#include "GameFramework/PlayerController.h"

void AOSMKIngameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (BulletSlotWidgetClass)
	{
		BulletSlotWidgetInstance = CreateWidget<UIngameBulletSlotWidget>(GetWorld(), BulletSlotWidgetClass);
		if (BulletSlotWidgetInstance)
		{
			BulletSlotWidgetInstance->AddToViewport();
			BulletSlotWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (BulletCountdownWidgetClass)
	{
		BulletCountdownWidgetInstance = CreateWidget<UBulletCountdownWidget>(GetWorld(), BulletCountdownWidgetClass);
		if (BulletCountdownWidgetInstance)
		{
			BulletCountdownWidgetInstance->AddToViewport();
			BulletCountdownWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (APlayerController* PC = GetOwningPlayerController())
	{
		PC->OnPossessedPawnChanged.AddDynamic(this, &AOSMKIngameHUD::OnPawnChanged);
	}
}

void AOSMKIngameHUD::SetHUDVisible(bool bVisible)
{
	if (BulletSlotWidgetInstance)
	{
		BulletSlotWidgetInstance->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	if (BulletCountdownWidgetInstance)
	{
		BulletCountdownWidgetInstance->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void AOSMKIngameHUD::OnPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (!BulletSlotWidgetInstance)
	{
		return;
	}

	const bool bIsPlayerCharacter = Cast<APlayerCharacter>(NewPawn) != nullptr;
	BulletSlotWidgetInstance->SetVisibility(bIsPlayerCharacter ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	if (BulletCountdownWidgetInstance)
	{
		BulletCountdownWidgetInstance->SetVisibility(bIsPlayerCharacter ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}