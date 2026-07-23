#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StageDataExtractorLibrary.generated.h"

UCLASS()
class OSMK_API UStageDataExtractorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void ExtractStaticMeshFromLevels(class UDataTable* TargetDataTable, TArray<TSoftObjectPtr<UWorld>> TargetLevels);

	UFUNCTION(BlueprintCallable)
	static void ExtractEnemyFromLevels(class UDataTable* TargetDataTable, TArray<TSoftObjectPtr<UWorld>> TargetLevels);
};