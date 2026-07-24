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

	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStageData, StageStaticMeshData))
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
	
	TMap<FName, TArray<FName>> ExistingBullets;
	for (const auto& Config : StageConfigs)
	{
		ExistingBullets.Add(Config.StageRowName, Config.UnlockedBulletRowNames);
	}

	StageConfigs.Empty();

	for (const FName& RowName : RowNames)
	{
		FStageLevelConfig NewConfig;
		NewConfig.StageRowName = RowName;
		
		if (TArray<FName>* FoundBullets = ExistingBullets.Find(RowName))
		{
			NewConfig.UnlockedBulletRowNames = *FoundBullets;
		}

		StageConfigs.Add(NewConfig);
	}
}
#endif