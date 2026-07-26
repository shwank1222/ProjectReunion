#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SliderWidget.generated.h"

class USlider;
class UProgressBar;
class UEditableTextBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSliderValueChanged, float, Value);

UCLASS()
class OSMK_API USliderWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Slider")
	void SetValue(float NewValue);

	UFUNCTION(BlueprintPure, Category="Slider")
	float GetValue() const;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void HandleSliderValueChanged(float NormalizedValue);

	UFUNCTION()
	void HandleTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	bool bSuppressBroadcast = false;

	float ToActual(float Normalized) const;
	float ToNormalized(float Actual) const;
	void SyncText(float ActualValue);
	void ApplyNormalized(float Normalized);

public:
	UPROPERTY(BlueprintAssignable, Category="Slider")
	FOnSliderValueChanged OnValueChanged;
	
protected:
	UPROPERTY(meta=(BindWidget))
	USlider* InputSlider = nullptr;

	UPROPERTY(meta=(BindWidget))
	UProgressBar* FillBar = nullptr;

	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* ValueTextBox = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slider")
	float CurrentValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slider")
	float MinValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slider")
	float MaxValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slider")
	bool bDisplayAsPercentage = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slider", meta=(EditCondition="!bDisplayAsPercentage"))
	int32 DecimalPlaces = 2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slider|Sound")
	class USoundBase* ValueChangedSound = nullptr;
};