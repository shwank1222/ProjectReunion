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
