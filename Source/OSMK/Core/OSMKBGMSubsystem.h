#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OSMKBGMSubsystem.generated.h"

UCLASS()
class OSMK_API UOSMKBGMSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void PlayBGM(USoundBase* NewBGM, float FadeInDuration = 1.0f);
	void StopBGM(float FadeOutDuration = 1.0f);

private:
	void OnWorldChanged(UWorld* OldWorld, const UWorld::InitializationValues IVS);
	void FadeIn(float Duration);
	void FadeOut(float Duration, TFunction<void()> OnComplete = nullptr);

private:
	UPROPERTY()
	class UAudioComponent* BGMAudioComponent = nullptr;

	UPROPERTY()
	USoundBase* PendingBGM = nullptr;

	float PendingFadeInDuration = 1.0f;

	FTimerHandle FadeTimerHandle;
	FDelegateHandle WorldChangedHandle;
};
