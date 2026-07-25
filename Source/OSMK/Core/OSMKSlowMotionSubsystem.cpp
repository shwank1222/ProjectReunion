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
	APostProcessVolume* PostProcessVolume = GetPostProcessVolume(); 
	if (!IsValid(PostProcessVolume))
	{
		return;
	}
	
	if (!PostProcessVolume->Settings.WeightedBlendables.Array.IsEmpty())
	{
		PostProcessVolume->Settings.WeightedBlendables.Array[0].Weight = 1.0f;
	}
}

void UOSMKSlowMotionSubsystem::RestoreGimmickHighlight() const
{
	APostProcessVolume* PostProcessVolume = GetPostProcessVolume(); 
	if (!IsValid(PostProcessVolume))
	{
		return;
	}
	
	if (!PostProcessVolume->Settings.WeightedBlendables.Array.IsEmpty())
	{
		PostProcessVolume->Settings.WeightedBlendables.Array[0].Weight = 0.0f;
	}
}

APostProcessVolume* UOSMKSlowMotionSubsystem::GetPostProcessVolume() const
{
	AActor* FindActor = UGameplayStatics::GetActorOfClass(this, APostProcessVolume::StaticClass());
	return Cast<APostProcessVolume>(FindActor);
}
