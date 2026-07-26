#include "UI/StageResult/StageFailWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/OSMKInGameGameMode.h"

void UStageFailWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Retry)
	{
		Btn_Retry->OnClicked.AddDynamic(this, &UStageFailWidget::OnRetryClicked);
	}

	if (Btn_Title)
	{
		Btn_Title->OnClicked.AddDynamic(this, &UStageFailWidget::OnTitleClicked);
	}

	if (Anim_FadeIn)
	{
		PlayAnimation(Anim_FadeIn);
	}
}

void UStageFailWidget::OnRetryClicked()
{
	bRetryPending = true;

	if (Anim_FadeOut)
	{
		FWidgetAnimationDynamicEvent Delegate;
		Delegate.BindDynamic(this, &UStageFailWidget::OnFadeOutFinished);
		BindToAnimationFinished(Anim_FadeOut, Delegate);
		PlayAnimation(Anim_FadeOut);
	}
	else
	{
		OnFadeOutFinished();
	}
}

void UStageFailWidget::OnTitleClicked()
{
	bRetryPending = false;

	if (Anim_FadeOut)
	{
		FWidgetAnimationDynamicEvent Delegate;
		Delegate.BindDynamic(this, &UStageFailWidget::OnFadeOutFinished);
		BindToAnimationFinished(Anim_FadeOut, Delegate);
		PlayAnimation(Anim_FadeOut);
	}
	else
	{
		OnFadeOutFinished();
	}
}

void UStageFailWidget::OnFadeOutFinished()
{
	RemoveFromParent();

	if (bRetryPending)
	{
		if (AOSMKInGameGameMode* GM = Cast<AOSMKInGameGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->RetryStage();
		}
	}
	else
	{
		UGameplayStatics::OpenLevel(this, FName("L_Title"));
	}
}
