#include "Core/OSMKSlowMotionSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UOSMKSlowMotionSubsystem::ApplySlowMotion(float TimeDilation, float Duration)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (TimeDilation <= 0.f || Duration <= 0.f)
	{
		return;
	}

	if (bIsActive)
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
	}

	UGameplayStatics::SetGlobalTimeDilation(World, TimeDilation);
	bIsActive = true;

	World->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&UOSMKSlowMotionSubsystem::RestoreTimeDilation,
		Duration * TimeDilation,
		false
	);
}

void UOSMKSlowMotionSubsystem::RestoreTimeDilation()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameplayStatics::SetGlobalTimeDilation(World, 1.f);
	World->GetTimerManager().ClearTimer(TimerHandle);
	bIsActive = false;
}

void UOSMKSlowMotionSubsystem::ApplyGimmickHighlight() const
{
	UpdatePostProcessMaterialValue(0, 1.0f);
}

void UOSMKSlowMotionSubsystem::RestoreGimmickHighlight() const
{
	UpdatePostProcessMaterialValue(0, 0.0f);
}

void UOSMKSlowMotionSubsystem::UpdatePostProcessMaterialValue(const int32 Index, const float Value) const
{
	if (APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(
		UGameplayStatics::GetActorOfClass(GetWorld(), APostProcessVolume::StaticClass())))
	{
		if (!PostProcessVolume->Settings.WeightedBlendables.Array.IsEmpty())
		{
			PostProcessVolume->Settings.WeightedBlendables.Array[Index].Weight = Value;
		}
	}
}
