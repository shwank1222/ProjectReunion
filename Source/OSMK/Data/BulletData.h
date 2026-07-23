#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BulletData.generated.h"

USTRUCT(BlueprintType)
struct OSMK_API FBulletData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	TSoftObjectPtr<UTexture2D> BulletIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	TSoftClassPtr<AActor> BulletBlueprint = nullptr;
};