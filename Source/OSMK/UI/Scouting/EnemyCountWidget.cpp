#include "EnemyCountWidget.h"
#include "Components/TextBlock.h"
#include "Core/OSMKGameState.h"

void UEnemyCountWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CachedGameState = Cast<AOSMKGameState>(GetWorld()->GetGameState());
	if (CachedGameState.IsValid())
	{
		CachedGameState->OnEnemyCountChanged.AddDynamic(this, &UEnemyCountWidget::UpdateEnemyCount);
		UpdateEnemyCount();
	}
}

void UEnemyCountWidget::NativeDestruct()
{
	if (CachedGameState.IsValid())
	{
		CachedGameState->OnEnemyCountChanged.RemoveDynamic(this, &UEnemyCountWidget::UpdateEnemyCount);
	}

	Super::NativeDestruct();
}

void UEnemyCountWidget::UpdateEnemyCount()
{
	if (!CachedGameState.IsValid())
	{
		return;
	}

	const int32 CurrentEnemyCount = CachedGameState->EnemyCount;
	if (CurrentEnemyCount > TotalEnemyCount)
	{
		TotalEnemyCount = CurrentEnemyCount;
	}

	if (Text_EnemyCount)
	{
		Text_EnemyCount->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentEnemyCount, TotalEnemyCount)));
	}
}
