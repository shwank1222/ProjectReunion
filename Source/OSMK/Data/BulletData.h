#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BulletData.generated.h"

class ABulletBase;

USTRUCT(BlueprintType)
struct OSMK_API FBulletData : public FDataTableRowHandle
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	FText BulletName = FText::GetEmpty();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	FText BulletDescription = FText::GetEmpty();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	TSoftObjectPtr<UTexture2D> BulletIcon = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	TSoftObjectPtr<UTexture2D> CylinderIcon = nullptr;      

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	TSoftClassPtr<ABulletBase> BulletBlueprint = nullptr;
};