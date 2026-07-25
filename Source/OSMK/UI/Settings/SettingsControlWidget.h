#pragma once

#include "CoreMinimal.h"
#include "SettingsTabWidgetBase.h"
#include "SettingsControlWidget.generated.h"

class USliderWidget;

UCLASS()
class OSMK_API USettingsControlWidget : public USettingsTabWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeOnOpenTab(class UOSMKSaveGame* InSaveGame) override;
	virtual void NativeOnConfirmSettings(class UOSMKSaveGame* OutSaveGame) override;
	virtual void NativeOnCancelSettings() override;
	virtual void NativeOnResetToDefault() override;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnSensitivityChanged(float Value);

private:
	void ApplySensitivitySettings(float Value);
	
protected:
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	USliderWidget* SensitivitySlider = nullptr;

private:
	float BackupSensitivity = 1.0f;
};