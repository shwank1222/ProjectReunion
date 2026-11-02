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
	SetGimmickHighlightWeight(1.0f);
}

void UOSMKSlowMotionSubsystem::RestoreGimmickHighlight() const
{
	SetGimmickHighlightWeight(0.0f);
}

void UOSMKSlowMotionSubsystem::SetGimmickHighlightWeight(const float Value) const
{
	static const FSoftObjectPath HighlightMaterialPath(TEXT("/Game/_Materials/PostProcess/PPI_PostProcess.PPI_PostProcess"));

	TArray<AActor*> Volumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), Volumes);

	for (AActor* Actor : Volumes)
	{
		APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(Actor);
		if (!PostProcessVolume)
		{
			continue;
		}

		for (FWeightedBlendable& Blendable : PostProcessVolume->Settings.WeightedBlendables.Array)
		{
			if (Blendable.Object && FSoftObjectPath(Blendable.Object) == HighlightMaterialPath)
			{
				Blendable.Weight = Value;
			}
		}
	}
}
