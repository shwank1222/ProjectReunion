#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SoundData.generated.h"

UCLASS(BlueprintType)
class OSMK_API USoundData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM|Layer")
	TSoftObjectPtr<USoundBase> CalmLayerBGM = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM|Layer")
	TSoftObjectPtr<USoundBase> CombatLayerBGM = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM|Layer", meta = (ClampMin = "0.0"))
	float LayerCrossfadeDuration = 2.0f;
};
