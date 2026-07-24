#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StageStaticMeshData.generated.h"

USTRUCT(BlueprintType)
struct FStageStaticMeshItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	TSoftObjectPtr<class UStaticMesh> StaticMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	FTransform Transform = FTransform::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	FName CollisionProfileName = FName("NoCollision");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	TArray<TSoftObjectPtr<UMaterialInterface>> Materials;
};

USTRUCT(BlueprintType)
struct FStageStaticMeshData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	TArray<FStageStaticMeshItem> MeshList;
};