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

	void SetCombatLayer(bool bCombat);

private:
	void OnWorldChanged(UWorld* OldWorld, const UWorld::InitializationValues IVS);
	bool IsLayerPlaying() const;
	void StartLayerBGM(USoundBase* CalmSound, USoundBase* CombatSound, float FadeInDuration);
	float GetLayerCrossfadeDuration() const;

private:
	UPROPERTY()
	class UAudioComponent* CalmLayerComponent = nullptr;

	UPROPERTY()
	class UAudioComponent* CombatLayerComponent = nullptr;

	bool bCombatLayer = false;

	FDelegateHandle WorldChangedHandle;
};
