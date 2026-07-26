#include "StageClearWidget.h"

void UStageClearWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Anim_FadeIn)
	{
		PlayAnimation(Anim_FadeIn);
	}
}

void UStageClearWidget::PlayFadeOut()
{
	if (Anim_FadeOut)
	{
		FWidgetAnimationDynamicEvent Delegate;
		Delegate.BindDynamic(this, &UStageClearWidget::OnFadeOutFinished);
		BindToAnimationFinished(Anim_FadeOut, Delegate);
		PlayAnimation(Anim_FadeOut);
	}
	else
	{
		RemoveFromParent();
	}
}

void UStageClearWidget::OnFadeOutFinished()
{
	RemoveFromParent();
}
