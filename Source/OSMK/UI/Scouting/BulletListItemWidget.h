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

	void Init(FName InRowName, UTexture2D* InIcon);

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

private:
	FName RowName = NAME_None;
};
