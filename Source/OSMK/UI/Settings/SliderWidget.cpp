#include "SliderWidget.h"
#include "Components/Slider.h"
#include "Components/ProgressBar.h"
#include "Components/EditableTextBox.h"

float USliderWidget::ToActual(float Normalized) const
{
	return FMath::Lerp(MinValue, MaxValue, FMath::Clamp(Normalized, 0.f, 1.f));
}

float USliderWidget::ToNormalized(float Actual) const
{
	const float Range = MaxValue - MinValue;
	if (FMath::IsNearlyZero(Range))
	{
		return 0.f;
	}
	return FMath::Clamp((Actual - MinValue) / Range, 0.f, 1.f);
}

void USliderWidget::SyncText(float ActualValue)
{
	if (!ValueTextBox)
	{
		return;
	}

	if (bDisplayAsPercentage)
	{
		ValueTextBox->SetText(FText::AsNumber(FMath::RoundToInt(ActualValue * 100.f)));
	}
	else
	{
		FNumberFormattingOptions Opts;
		Opts.MinimumFractionalDigits = DecimalPlaces;
		Opts.MaximumFractionalDigits = DecimalPlaces;
		ValueTextBox->SetText(FText::AsNumber(ActualValue, &Opts));
	}
}

void USliderWidget::ApplyNormalized(float Normalized)
{
	if (FillBar)
	{
		FillBar->SetPercent(Normalized);
	}
	if (InputSlider)
	{
		InputSlider->SetValue(Normalized);
	}
	SyncText(ToActual(Normalized));
}

void USliderWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	ApplyNormalized(ToNormalized(CurrentValue));
}

void USliderWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (InputSlider)
	{
		InputSlider->OnValueChanged.AddDynamic(this, &USliderWidget::HandleSliderValueChanged);
	}

	if (ValueTextBox)
	{
		ValueTextBox->OnTextCommitted.AddDynamic(this, &USliderWidget::HandleTextCommitted);
	}

	ApplyNormalized(ToNormalized(CurrentValue));
}

void USliderWidget::SetValue(float NewValue)
{
	CurrentValue = FMath::Clamp(NewValue, MinValue, MaxValue);
	ApplyNormalized(ToNormalized(CurrentValue));
}

float USliderWidget::GetValue() const
{
	return CurrentValue;
}

void USliderWidget::HandleSliderValueChanged(float NormalizedValue)
{
	CurrentValue = ToActual(NormalizedValue);
	if (FillBar)
	{
		FillBar->SetPercent(NormalizedValue);
	}
	SyncText(CurrentValue);
	OnValueChanged.Broadcast(CurrentValue);
}

void USliderWidget::HandleTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnCleared)
	{
		SyncText(CurrentValue);
		return;
	}

	const FString Str = Text.ToString();
	if (!Str.IsNumeric())
	{
		SyncText(CurrentValue);
		return;
	}

	float Parsed = FCString::Atof(*Str);
	if (bDisplayAsPercentage)
	{
		Parsed /= 100.f;
	}

	SetValue(FMath::Clamp(Parsed, MinValue, MaxValue));
	OnValueChanged.Broadcast(CurrentValue);
}