#pragma once

#include "CoreMinimal.h"
#include "SettingsTabWidgetBase.h"
#include "SettingsLanguageWidget.generated.h"

UCLASS()
class OSMK_API USettingsLanguageWidget : public USettingsTabWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeOnOpenTab(class UOSMKSaveGame* InSaveGame) override;
	virtual void NativeOnConfirmSettings(class UOSMKSaveGame* OutSaveGame) override;
	virtual void NativeOnCancelSettings() override;
	virtual void NativeOnResetToDefault() override;
	
protected:
	virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void OnLanguageSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	void ApplyLanguageSettings(const FString& CultureCode);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UComboBoxString* LanguageComboBox = nullptr;

private:
	FString PendingCulture = TEXT("");
	FString BackupCulture = TEXT("");
};