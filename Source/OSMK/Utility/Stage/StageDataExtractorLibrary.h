#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StageDataExtractorLibrary.generated.h"

class UStageData;

UCLASS()
class OSMK_API UStageDataExtractorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Stage Extractor")
	static void ExtractFromStageData(UStageData* StageData, TArray<TSoftObjectPtr<UWorld>> TargetLevels);
};
