#pragma once

#include "CoreMinimal.h"
#include "Data/Stage/StageExtractRule.h"
#include "StageExtractRule_ScoutCamera.generated.h"

UCLASS(DisplayName = "Scout Camera")
class OSMK_API UStageExtractRule_ScoutCamera : public UStageExtractRule
{
	GENERATED_BODY()

public:
	UStageExtractRule_ScoutCamera();

#if WITH_EDITOR
	virtual bool MatchesActor(const AActor* Actor) const override;
	virtual void FillItem(const AActor* Actor, FStageExtractedItem& OutItem) const override;
#endif

protected:
	virtual void ConfigureSpawnedActor(AActor* SpawnedActor, const FStageExtractedItem& Item, FStageSpawnContext& Ctx) const override;
};
