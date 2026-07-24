#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "OSMK/Interactable/Gimmick/GimmickBase.h"
#include "StageGimmickData.generated.h"

USTRUCT(BlueprintType)
struct FStageGimmickItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	TSoftClassPtr<class AGimmickBase> GimmickClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	FTransform Transform = FTransform::Identity;
};

USTRUCT(BlueprintType)
struct FStageGimmickData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	TArray<FStageGimmickItem> GimmickList;
};