#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsWidget.generated.h"

UCLASS()
class OSMK_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	UFUNCTION()
	void OnTabButtonClicked();

	UFUNCTION()
	void OnConfirmClicked();

	UFUNCTION()
	void OnResetClicked();

	UFUNCTION()
	void OnCancelClicked();
	
	UFUNCTION()
	void CloseSettingsWidget();

	void SwitchToTab(int32 TabIndex);

	void InitializeTabButtons();

protected:
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UWidgetSwitcher* TabSwitcher = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UButton* ConfirmButton = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UButton* ResetButton = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UButton* CancelButton = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UPanelWidget* TabButtonContainer = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class USettingsGraphicWidget* GraphicTabWidget = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class USettingsSoundWidget* SoundTabWidget = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class USettingsControlWidget* ControlTabWidget = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class USettingsLanguageWidget* LanguageTabWidget = nullptr;

private:
	UPROPERTY()
	TArray<class USettingsTabWidgetBase*> SettingTabs;

	UPROPERTY()
	TArray<class UButton*> TabButtons;
};