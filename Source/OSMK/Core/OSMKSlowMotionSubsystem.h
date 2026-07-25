#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OSMKSlowMotionSubsystem.generated.h"

UCLASS()
class OSMK_API UOSMKSlowMotionSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// Applies a global slow-motion effect (TimeDilation: The scale of time to apply, Duration: The duration of the effect in seconds).
	UFUNCTION(BlueprintCallable)
	void ApplySlowMotion(float TimeDilation, float Duration);

	UFUNCTION(BlueprintCallable)
	void RestoreTimeDilation();

	UFUNCTION(BlueprintPure)
	bool IsInSlowMotion() const { return bIsActive; }
	
	void ApplyGimmickHighlight() const;
	void RestoreGimmickHighlight() const;

private:
	APostProcessVolume* GetPostProcessVolume() const;
	
	FTimerHandle TimerHandle;
	bool bIsActive = false;
};
