#include "KillFeedbackWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UKillFeedbackWidget::InitFeedback(const FText& Text)
{
	if (Text_Feedback)
	{
		Text_Feedback->SetText(Text);
	}

	if (Anim_ShowFeedback)
	{
		FWidgetAnimationDynamicEvent EndDelegate;
		EndDelegate.BindDynamic(this, &UKillFeedbackWidget::OnFadeOutFinished);
		BindToAnimationFinished(Anim_ShowFeedback, EndDelegate);

		PlayAnimation(Anim_ShowFeedback);
	}
}

void UKillFeedbackWidget::OnFadeOutFinished()
{
	RemoveFromParent();
}