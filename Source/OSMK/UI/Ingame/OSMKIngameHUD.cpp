#include "OSMKIngameHUD.h"

#include "BulletCountdownWidget.h"
#include "IngameBulletSlotWidget.h"
#include "KillFeedbackListWidget.h"
#include "Character/PlayerCharacter.h"
#include "Core/OSMKGameState.h"
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

	if (KillFeedbackListWidgetClass)
	{
		KillFeedbackListWidgetInstance = CreateWidget<UKillFeedbackListWidget>(GetWorld(), KillFeedbackListWidgetClass);
		if (KillFeedbackListWidgetInstance)
		{
			KillFeedbackListWidgetInstance->AddToViewport();
			KillFeedbackListWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (APlayerController* PC = GetOwningPlayerController())
	{
		PC->OnPossessedPawnChanged.AddDynamic(this, &AOSMKIngameHUD::OnPawnChanged);
	}

	if (AOSMKGameState* GS = GetWorld()->GetGameState<AOSMKGameState>())
	{
		CachedEnemyCount = GS->EnemyCount;
		GS->OnEnemyCountChanged.AddDynamic(this, &AOSMKIngameHUD::HandleEnemyCountChanged);
	}
}

void AOSMKIngameHUD::HandleEnemyCountChanged()
{
	if (AOSMKGameState* GS = GetWorld()->GetGameState<AOSMKGameState>())
	{
			if (GS->EnemyCount < CachedEnemyCount)
		{
			ShowKillFeedback();
		}
		CachedEnemyCount = GS->EnemyCount;
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

	if (KillFeedbackListWidgetInstance)
	{
		KillFeedbackListWidgetInstance->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void AOSMKIngameHUD::ShowKillFeedback()
{
	if (KillFeedbackListWidgetInstance)
	{
		KillFeedbackListWidgetInstance->AddKillFeedback();
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

	if (KillFeedbackListWidgetInstance)
	{
		KillFeedbackListWidgetInstance->SetVisibility(bIsPlayerCharacter ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}