#pragma once

#include "CoreMinimal.h"
#include "StageExtractedItem.generated.h"

UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EStageExtractField : uint16
{
	None       = 0 UMETA(Hidden),
	Transform  = 1 << 0,
	Class      = 1 << 1,
	Mesh       = 1 << 2,
	Collision  = 1 << 3,
	Materials  = 1 << 4,
	LevelAsset = 1 << 5,
	SpringArm  = 1 << 6,
	Properties = 1 << 7 UMETA(ToolTip = "Capture and restore the properties selected in SyncProperties."),
	Functions  = 1 << 8 UMETA(ToolTip = "Call the functions selected in PostSpawnFunctions on the tick after spawning."),
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
	TMap<FName, FString> PropertyValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData|Brush")
	bool bHasBrushBounds = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData|Brush")
	FVector BrushBoxCenter = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData|Brush")
	FVector BrushBoxExtent = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData|Brush")
	FName BrushCollisionProfile = NAME_None;
};

USTRUCT(BlueprintType)
struct FStageExtractedGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StageData")
	TArray<FStageExtractedItem> Items;
};
