#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "OSMKStageExtractorSettings.generated.h"

class UStageData;

UCLASS(Config = EditorPerProjectUserSettings, defaultconfig, meta = (DisplayName = "Stage Extractor"))
class UOSMKStageExtractorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Stage Extractor")
	TArray<TSoftObjectPtr<UWorld>> TargetLevels;

	UPROPERTY(Config, EditAnywhere, Category = "Stage Extractor")
	TSoftObjectPtr<UStageData> TargetStageData = nullptr;
};
