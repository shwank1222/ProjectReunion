#include "Data/Stage/StageExtractedCache.h"

void UStageExtractedCache::Write(FName RuleId, FName LevelName, const FStageExtractedGroup& Group)
{
	FStageExtractedCacheEntry& Entry = Data.FindOrAdd(RuleId);
	Entry.ByLevel.Add(LevelName, Group);

#if WITH_EDITOR
	MarkPackageDirty();
#endif
}

const FStageExtractedGroup* UStageExtractedCache::Read(FName RuleId, FName LevelName) const
{
	if (const FStageExtractedCacheEntry* Entry = Data.Find(RuleId))
	{
		return Entry->ByLevel.Find(LevelName);
	}
	return nullptr;
}

void UStageExtractedCache::ClearRule(FName RuleId)
{
	Data.Remove(RuleId);

#if WITH_EDITOR
	MarkPackageDirty();
#endif
}

void UStageExtractedCache::ClearAll()
{
	Data.Empty();

#if WITH_EDITOR
	MarkPackageDirty();
#endif
}
