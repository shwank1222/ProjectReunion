#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillFeedbackWidget.generated.h"

UCLASS()
class OSMK_API UKillFeedbackWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitFeedback(const FText& Text);

protected:
	UFUNCTION()
	void OnFadeOutFinished();
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Feedback = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* Anim_ShowFeedback = nullptr;
};