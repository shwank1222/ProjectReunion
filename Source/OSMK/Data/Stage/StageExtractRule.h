#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Data/Stage/StageExtractedItem.h"
#include "StageExtractRule.generated.h"

class UStageExtractedCache;
class UWorld;
class ULevelStreamingDynamic;

struct FStageSpawnContext
{
	UWorld* World = nullptr;
	FName LevelName = NAME_None;
	const UStageExtractedCache* Cache = nullptr;

	TFunction<void(AActor*)> TrackActor;
	TFunction<void(ULevelStreamingDynamic*)> TrackStreaming;
	TFunction<void(const FTransform&)> SetPlayerStart;
	TFunction<void(AActor*)> SetScoutCamera;
	TFunction<void()> BeginAsync;
	TFunction<void()> EndAsync;
};

UCLASS(Abstract, EditInlineNew, Blueprintable, DefaultToInstanced, CollapseCategories)
class OSMK_API UStageExtractRule : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule")
	FName RuleId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule")
	TSubclassOf<AActor> TargetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule", meta = (Bitmask, BitmaskEnum = "/Script/OSMK.EStageExtractField"))
	int32 Fields = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule", meta = (ToolTip = "If true, only actors with exactly TargetClass match. If false, subclasses also match."))
	bool bMatchExactClass = true;

#if WITH_EDITOR
	virtual void ExtractFromWorld(UWorld* World, FName LevelName, UStageExtractedCache* Cache) const;

protected:
	virtual bool MatchesActor(const AActor* Actor) const;
	virtual void FillItem(const AActor* Actor, FStageExtractedItem& OutItem) const;
#endif

public:
	virtual void SpawnStage(FStageSpawnContext& Ctx) const;

protected:
	bool HasField(EStageExtractField Field) const { return (Fields & static_cast<int32>(Field)) != 0; }

	virtual void SpawnItem(FStageSpawnContext& Ctx, const FStageExtractedItem& Item) const;
	virtual void ConfigureSpawnedActor(AActor* SpawnedActor, const FStageExtractedItem& Item, FStageSpawnContext& Ctx) const;
	void ApplyPCGExtent(AActor* SpawnedActor, const FStageExtractedItem& Item, FStageSpawnContext& Ctx) const;
};
