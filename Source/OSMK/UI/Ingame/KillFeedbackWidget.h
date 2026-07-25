#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillFeedbackWidget.generated.h"

UCLASS()
class OSMK_API UKillFeedbackWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Feedback")
	void ShowRandomFeedback();

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Feedback = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* Anim_ShowFeedback = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	class UDataTable* KillFeedbackDataTable = nullptr;
};