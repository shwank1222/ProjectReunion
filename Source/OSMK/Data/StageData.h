#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StageData.generated.h"

UCLASS()
class OSMK_API UStageData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSoftObjectPtr<UWorld>> StageLevels;
};