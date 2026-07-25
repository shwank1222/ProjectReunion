#include "KillFeedbackListWidget.h"
#include "KillFeedbackWidget.h"
#include "Components/VerticalBox.h"
#include "Data/KillFeedbackData.h"

void UKillFeedbackListWidget::AddKillFeedback()
{
	if (!KillFeedbackWidgetClass || !VBox_FeedbackList || !KillFeedbackDataTable)
	{
		return;
	}

	TArray<FName> RowNames = KillFeedbackDataTable->GetRowNames();
	if (RowNames.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0, RowNames.Num() - 1);
		if (const FKillFeedbackData* Row = KillFeedbackDataTable->FindRow<FKillFeedbackData>(RowNames[RandomIndex], TEXT("")))
		{
			UKillFeedbackWidget* NewFeedback = CreateWidget<UKillFeedbackWidget>(this, KillFeedbackWidgetClass);
			if (NewFeedback)
			{
				NewFeedback->InitFeedback(Row->FeedbackText);
				VBox_FeedbackList->InsertChildAt(0, NewFeedback);
			}
		}
	}
}
