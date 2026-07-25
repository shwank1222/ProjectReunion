#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BulletListItemWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnBulletItemClicked, FName);

UCLASS()
class OSMK_API UBulletListItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnBulletItemClicked OnBulletItemClicked;

	void Init(FName InRowName, UTexture2D* InIcon, const FText& InBulletName, const FText& InBulletDescription);
	void SetCount(int32 Count);

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnItemClicked();
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Item = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Icon = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Count = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Name = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Tooltip")
	TSubclassOf<class UBulletTooltipWidget> TooltipWidgetClass = nullptr;

private:
	FName RowName = NAME_None;
};
