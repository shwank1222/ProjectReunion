#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Pawn.h"
#include "StageData.generated.h"

class UStageExtractRule;
class UStageExtractedCache;

USTRUCT(BlueprintType)
struct FStageLevelData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> Level = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer UnlockedBullets = FGameplayTagContainer();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	TArray<FVector> EnemySpawnLocations;
};

USTRUCT(BlueprintType)
struct FStageLevelConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	FName StageRowName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TSoftObjectPtr<UWorld> Level = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet")
	TMap<FName, int32> BulletCounts;
};

UCLASS()
class OSMK_API UStageData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* BulletDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TSoftClassPtr<APawn> PlayerCharacterClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TSoftObjectPtr<UWorld> TitleLevel = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TSoftObjectPtr<UWorld> InGameLevel = nullptr;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Extractor")
	TArray<TObjectPtr<UStageExtractRule>> Rules;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extractor")
	TSoftObjectPtr<UStageExtractedCache> Cache = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	TArray<FStageLevelConfig> StageConfigs;
};
