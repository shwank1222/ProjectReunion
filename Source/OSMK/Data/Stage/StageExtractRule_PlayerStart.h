#pragma once

#include "CoreMinimal.h"
#include "Data/Stage/StageExtractRule.h"
#include "StageExtractRule_PlayerStart.generated.h"

UCLASS(DisplayName = "Player Start (Transform Only)")
class OSMK_API UStageExtractRule_PlayerStart : public UStageExtractRule
{
	GENERATED_BODY()

public:
	UStageExtractRule_PlayerStart();

protected:
	virtual void SpawnItem(FStageSpawnContext& Ctx, const FStageExtractedItem& Item) const override;
};
