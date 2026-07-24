#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Pawn.h"
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

USTRUCT(BlueprintType)
struct FStageLevelConfig
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage")
	FName StageRowName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet")
	TArray<FName> UnlockedBulletRowNames;
};

UCLASS()
class OSMK_API UStageData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	void RefreshStageConfigs();
#endif
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet")
	UDataTable* BulletDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	TSoftClassPtr<AActor> EnemyClass = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scout Camera")
	TSoftClassPtr<AActor> ScoutCameraClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
	TSoftClassPtr<APawn> PlayerCharacterClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TSoftObjectPtr<UWorld> InGameLevel = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TObjectPtr<UDataTable> StageStaticMeshData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TObjectPtr<UDataTable> StageEnemyData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TObjectPtr<UDataTable> StageGimmickData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TObjectPtr<UDataTable> StageActorData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TObjectPtr<UDataTable> StageScoutCameraData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	TArray<FStageLevelConfig> StageConfigs;
};