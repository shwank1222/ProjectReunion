#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StageActorData.generated.h"

USTRUCT(BlueprintType)
struct FStageActorItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<AActor> ActorClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform Transform = FTransform::Identity;
};

USTRUCT(BlueprintType)
struct FStageNavMeshItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform Transform = FTransform::Identity;
};

USTRUCT(BlueprintType)
struct FStageActorData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger")
	TArray<FStageActorItem> TriggerList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NavMesh")
	TArray<FStageNavMeshItem> NavMeshList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Start")
	FTransform PlayerStartTransform = FTransform::Identity;
};