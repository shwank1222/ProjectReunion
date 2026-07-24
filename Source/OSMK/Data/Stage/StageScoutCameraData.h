#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StageScoutCameraData.generated.h"

USTRUCT(BlueprintType)
struct FStageScoutCameraData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scout Camera")
	FTransform CameraTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scout Camera")
	float SpringArmLength = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scout Camera")
	FVector SpringArmSocketOffset = FVector::ZeroVector;
};
