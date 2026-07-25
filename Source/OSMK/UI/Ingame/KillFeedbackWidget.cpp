#include "KillFeedbackWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Data/KillFeedbackData.h"
#include "Kismet/KismetMathLibrary.h"

void UKillFeedbackWidget::ShowRandomFeedback()
{
	if (!KillFeedbackDataTable || !Text_Feedback)
	{
		return;
	}

	TArray<FName> RowNames = KillFeedbackDataTable->GetRowNames();
	if (RowNames.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0, RowNames.Num() - 1);
		const FName SelectedRowName = RowNames[RandomIndex];

		if (const FKillFeedbackData* Row = KillFeedbackDataTable->FindRow<FKillFeedbackData>(SelectedRowName, TEXT("KillFeedback")))
		{
			Text_Feedback->SetText(Row->FeedbackText);

			if (Anim_ShowFeedback)
			{
				StopAnimation(Anim_ShowFeedback);
				PlayAnimation(Anim_ShowFeedback);
			}
		}
	}
}