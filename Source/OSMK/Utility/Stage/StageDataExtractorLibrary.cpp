#include "Utility/Stage/StageDataExtractorLibrary.h"
#include "Data/StageData.h"
#include "Data/Stage/StageExtractRule.h"
#include "Data/Stage/StageExtractedCache.h"
#include "Engine/World.h"

void UStageDataExtractorLibrary::ExtractFromStageData(UStageData* StageData, TArray<TSoftObjectPtr<UWorld>> TargetLevels)
{
#if WITH_EDITOR
	if (!StageData)
	{
		return;
	}

	UStageExtractedCache* Cache = StageData->Cache.LoadSynchronous();
	if (!Cache)
	{
		UE_LOG(LogTemp, Error, TEXT("[StageDataExtractor] Cache DataAsset is null on StageData"));
		return;
	}

	Cache->ClearAll();

	for (const TSoftObjectPtr<UWorld>& SoftWorld : TargetLevels)
	{
		if (SoftWorld.IsNull())
		{
			continue;
		}

		UWorld* LoadedWorld = SoftWorld.LoadSynchronous();
		if (!LoadedWorld)
		{
			continue;
		}

		const FName LevelName = FName(*LoadedWorld->GetName());

		for (const TObjectPtr<UStageExtractRule>& Rule : StageData->Rules)
		{
			if (!Rule)
			{
				continue;
			}
			Rule->ExtractFromWorld(LoadedWorld, LevelName, Cache);
		}
	}

	TMap<FName, TMap<FName, int32>> ExistingCounts;
	for (const FStageLevelConfig& Config : StageData->StageConfigs)
	{
		ExistingCounts.Add(Config.StageRowName, Config.BulletCounts);
	}

	StageData->StageConfigs.Empty();

	for (const TSoftObjectPtr<UWorld>& SoftWorld : TargetLevels)
	{
		if (SoftWorld.IsNull())
		{
			continue;
		}

		FStageLevelConfig NewConfig;
		NewConfig.StageRowName = FName(*SoftWorld.GetAssetName());
		NewConfig.Level = SoftWorld;

		if (TMap<FName, int32>* Found = ExistingCounts.Find(NewConfig.StageRowName))
		{
			NewConfig.BulletCounts = *Found;
		}

		StageData->StageConfigs.Add(NewConfig);
	}

	StageData->MarkPackageDirty();
#endif
}
