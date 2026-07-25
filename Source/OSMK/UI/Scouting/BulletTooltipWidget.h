#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BulletTooltipWidget.generated.h"

class UTextBlock;

UCLASS()
class OSMK_API UBulletTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetTooltipData(const FText& InBulletName, const FText& InBulletDescription);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_BulletName = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_BulletDescription = nullptr;
};