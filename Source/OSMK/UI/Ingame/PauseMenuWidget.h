#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UButton;
class UPopupConfirmWidget;

UCLASS()
class OSMK_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	UFUNCTION()
	void OnCloseClicked();

	UFUNCTION()
	void OnSettingsClicked();

	UFUNCTION()
	void OnQuitClicked();

	UFUNCTION()
	void OnQuitConfirmed();

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Close = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Settings = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_QuitToTitle = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> SettingsWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPopupConfirmWidget> PopupConfirmWidgetClass = nullptr;

private:
	bool bRestoreCursorOnClose = false;
};