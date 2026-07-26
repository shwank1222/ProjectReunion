#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageFailWidget.generated.h"

class UButton;

UCLASS()
class OSMK_API UStageFailWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnRetryClicked();

	UFUNCTION()
	void OnTitleClicked();

	UFUNCTION()
	void OnFadeOutFinished();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Retry = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Title = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_FadeIn = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_FadeOut = nullptr;

private:
	bool bRetryPending = false;
};
