#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SoundData.generated.h"

UCLASS(BlueprintType)
class OSMK_API USoundData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM")
	TMap<TSoftObjectPtr<UWorld>, TSoftObjectPtr<USoundBase>> LevelBGMMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM")
	TSoftObjectPtr<USoundBase> DefaultBGM = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TMap<FName, TSoftObjectPtr<USoundBase>> SkillSoundPool;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TSoftObjectPtr<USoundAttenuation> SkillSoundAttenuation = nullptr;
};