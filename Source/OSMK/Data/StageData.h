#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "StageData.generated.h"

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

UCLASS()
class OSMK_API UStageData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet")
	UDataTable* BulletDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	TSoftClassPtr<AActor> EnemyClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FStageLevelData> Stages;
};