#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "OSMKStageExtractorSettings.generated.h"

UCLASS(Config = EditorPerProjectUserSettings, defaultconfig, meta = (DisplayName = "Stage Extractor"))
class UOSMKStageExtractorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Stage Extractor")
	TArray<TSoftObjectPtr<UWorld>> TargetLevels;
};
