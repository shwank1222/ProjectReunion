#pragma once

#include "CoreMinimal.h"
#include "StageExtractedItem.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EStageExtractField : uint8
{
	None       = 0 UMETA(Hidden),
	Transform  = 1 << 0,
	Class      = 1 << 1,
	Mesh       = 1 << 2,
	Collision  = 1 << 3,
	Materials  = 1 << 4,
	LevelAsset = 1 << 5,
	SpringArm  = 1 << 6,
	PCGExtent  = 1 << 7,
};
ENUM_CLASS_FLAGS(EStageExtractField)

USTRUCT(BlueprintType)
struct FStageExtractedItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	TSoftClassPtr<AActor> ActorClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	FTransform Transform = FTransform::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	TSoftObjectPtr<class UStaticMesh> Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	FName CollisionProfile = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	TArray<TSoftObjectPtr<class UMaterialInterface>> Materials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	TSoftObjectPtr<UWorld> LevelAsset = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	float SpringArmLength = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	FVector SpringArmSocketOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	FVector PCGExtent = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FStageExtractedGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	TArray<FStageExtractedItem> Items;
};
