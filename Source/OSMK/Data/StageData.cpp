#include "StageData.h"

void UStageData::PostLoad()
{
	Super::PostLoad();
#if WITH_EDITOR
	RefreshStageConfigs();
#endif
}

#if WITH_EDITOR
void UStageData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedProp = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (ChangedProp == GET_MEMBER_NAME_CHECKED(UStageData, StageStaticMeshData)
		|| ChangedProp == GET_MEMBER_NAME_CHECKED(UStageData, BulletDataTable))
	{
		RefreshStageConfigs();
	}
}

void UStageData::RefreshStageConfigs()
{
	if (!StageStaticMeshData)
	{
		StageConfigs.Empty();
		return;
	}

	TArray<FName> RowNames = StageStaticMeshData->GetRowNames();

	TMap<FName, TMap<FName, int32>> ExistingCounts;
	for (const FStageLevelConfig& Config : StageConfigs)
	{
		ExistingCounts.Add(Config.StageRowName, Config.BulletCounts);
	}

	StageConfigs.Empty();

	for (const FName& RowName : RowNames)
	{
		FStageLevelConfig NewConfig;
		NewConfig.StageRowName = RowName;

		if (TMap<FName, int32>* Found = ExistingCounts.Find(RowName))
		{
			NewConfig.BulletCounts = *Found;
		}

		StageConfigs.Add(NewConfig);
	}
}
#endif
