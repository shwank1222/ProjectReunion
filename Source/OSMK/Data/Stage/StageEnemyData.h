#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StageEnemyData.generated.h"

USTRUCT(BlueprintType)
struct FStageEnemyItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator Rotation = FRotator::ZeroRotator;
};

USTRUCT(BlueprintType)
struct FStageEnemyData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FStageEnemyItem> EnemyList;
};
