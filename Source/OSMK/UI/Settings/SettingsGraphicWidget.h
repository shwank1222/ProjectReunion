#pragma once

#include "CoreMinimal.h"
#include "SettingsTabWidgetBase.h"
#include "SettingsGraphicWidget.generated.h"

struct FGraphicBackup
{
	EWindowMode::Type WindowMode;
	FIntPoint Resolution;
	int32 OverallQuality;
	float Brightness;
};

UCLASS()
class OSMK_API USettingsGraphicWidget : public USettingsTabWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeOnOpenTab(class UOSMKSaveGame* InSaveGame) override;
	virtual void NativeOnConfirmSettings(class UOSMKSaveGame* InSaveGame) override;
	virtual void NativeOnCancelSettings() override;
	virtual void NativeOnResetToDefault() override;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnScreenModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnQualityHighestClicked();

	UFUNCTION()
	void OnQualityHighClicked();

	UFUNCTION()
	void OnQualityMediumClicked();

	UFUNCTION()
	void OnQualityLowClicked();

	UFUNCTION()
	void OnBrightnessValueChanged(float Value);

private:
	void UpdateUIFromUserSettings();
	void ApplyQuality(int32 QualityLevel);
	void UpdateQualityButtonStyles();
	void ApplyBrightness(float Value);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UComboBoxString* ScreenModeComboBox = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UComboBoxString* ResolutionComboBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
	FLinearColor QualitySelectedColor = FLinearColor::White;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UButton* QualityButton_Highest = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UButton* QualityButton_High = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UButton* QualityButton_Medium = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UButton* QualityButton_Low = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class USliderWidget* BrightnessSlider = nullptr;


private:
	FGraphicBackup BackupSettings;

	int32 PendingQuality = 3;

	TArray<FButtonStyle> QualityNormalStyles;
};