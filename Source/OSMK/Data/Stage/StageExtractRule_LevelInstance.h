#pragma once

#include "CoreMinimal.h"
#include "Data/Stage/StageExtractRule.h"
#include "StageExtractRule_LevelInstance.generated.h"

UCLASS(DisplayName = "Level Instance")
class OSMK_API UStageExtractRule_LevelInstance : public UStageExtractRule
{
	GENERATED_BODY()

public:
	UStageExtractRule_LevelInstance();

#if WITH_EDITOR
	virtual bool MatchesActor(const AActor* Actor) const override;
	virtual void FillItem(const AActor* Actor, FStageExtractedItem& OutItem) const override;
#endif

protected:
	virtual void SpawnItem(FStageSpawnContext& Ctx, const FStageExtractedItem& Item) const override;
};
