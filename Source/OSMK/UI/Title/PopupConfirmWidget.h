#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopupConfirmWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConfirmClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCancelClicked);

UCLASS()
class OSMK_API UPopupConfirmWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPopupText(const FText& InMessage);

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void HandleConfirmClicked();

	UFUNCTION()
	void HandleCancelClicked();
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Popup")
	FOnConfirmClicked OnConfirmClicked;

	UPROPERTY(BlueprintAssignable, Category = "Popup")
	FOnCancelClicked OnCancelClicked;

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Message = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Confirm = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Cancel = nullptr;
};