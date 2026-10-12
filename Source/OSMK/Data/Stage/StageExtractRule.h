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

USTRUCT(BlueprintType)
struct FStageFunctionCall
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule", meta = (StageFunctionPicker))
	FName Function = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule",
		meta = (ToolTip = "Input arguments in parameter order, in text form (e.g. \"True\", \"1.5\", \"(X=1,Y=2,Z=3)\"). Missing arguments use default values."))
	TArray<FString> Arguments;
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

#if WITH_EDITORONLY_DATA
	UPROPERTY(meta = (DeprecatedProperty, DeprecationMessage = "Merged into SyncProperties."))
	TArray<FName> PCGParamNames_DEPRECATED;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule",
		meta = (EditCondition = "Fields & '/Script/OSMK.EStageExtractField::Properties'", EditConditionHides, StagePropertyPicker,
			ToolTip = "Properties of TargetClass (or its components) to capture on extract and restore on spawn."))
	TArray<FName> SyncProperties;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule",
		meta = (EditCondition = "Fields & '/Script/OSMK.EStageExtractField::Functions'", EditConditionHides,
			ToolTip = "Functions of TargetClass (or its components) to call in order on the tick after the actor is spawned."))
	TArray<FStageFunctionCall> PostSpawnFunctions;

	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void ExtractFromWorld(UWorld* World, FName LevelName, UStageExtractedCache* Cache) const;

	TArray<FString> GetPropertyOptions() const;

	TArray<FString> GetFunctionOptions() const;

protected:
	virtual bool MatchesActor(const AActor* Actor) const;
	virtual void FillItem(const AActor* Actor, FStageExtractedItem& OutItem) const;
	void CaptureParam(const AActor* Actor, FName ParamPath, TMap<FName, FString>& OutParams) const;
#endif

public:
	virtual void SpawnStage(FStageSpawnContext& Ctx) const;

protected:
	bool HasField(EStageExtractField Field) const { return (Fields & static_cast<int32>(Field)) != 0; }

	virtual void SpawnItem(FStageSpawnContext& Ctx, const FStageExtractedItem& Item) const;
	virtual void ConfigureSpawnedActor(AActor* SpawnedActor, const FStageExtractedItem& Item, FStageSpawnContext& Ctx) const;

	bool ApplyParams(AActor* SpawnedActor, const TMap<FName, FString>& Params, bool bComponentParams) const;
	void SchedulePostSpawnFunctions(AActor* SpawnedActor, FStageSpawnContext& Ctx) const;
	void CallPostSpawnFunctions(AActor* SpawnedActor) const;
	static void WaitForPCGGeneration(AActor* SpawnedActor, const TFunction<void()>& BeginAsync, const TFunction<void()>& EndAsync);

	static UObject* ResolveParamTarget(const AActor* Actor, FName ParamPath, FName& OutPropertyName);
	static void RefreshAfterParamChange(UObject* Target);
};
