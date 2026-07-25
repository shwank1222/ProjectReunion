#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillFeedbackListWidget.generated.h"

UCLASS()
class OSMK_API UKillFeedbackListWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Feedback")
	void AddKillFeedback();

protected:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* VBox_FeedbackList = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Feedback")
	TSubclassOf<class UKillFeedbackWidget> KillFeedbackWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	class UDataTable* KillFeedbackDataTable = nullptr;
};