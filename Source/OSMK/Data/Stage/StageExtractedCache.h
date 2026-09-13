#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/Stage/StageExtractedItem.h"
#include "StageExtractedCache.generated.h"

USTRUCT(BlueprintType)
struct FStageExtractedCacheEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	TMap<FName, FStageExtractedGroup> ByLevel;
};

UCLASS(BlueprintType)
class OSMK_API UStageExtractedCache : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cache")
	TMap<FName, FStageExtractedCacheEntry> Data;

	void Write(FName RuleId, FName LevelName, const FStageExtractedGroup& Group);
	const FStageExtractedGroup* Read(FName RuleId, FName LevelName) const;
	void ClearRule(FName RuleId);
	void ClearAll();
};
