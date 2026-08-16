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
	void Init(FName InRowName, UTexture2D* InIcon, const FText& InBulletName, const FText& InBulletDescription);
	void SetCount(int32 Count);
	void SetIconHidden(bool bHidden);
	void RestoreFromDrag();

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	
protected:

	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Icon = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Count = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Name = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Tooltip")
	TSubclassOf<class UBulletTooltipWidget> TooltipWidgetClass = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "DragDrop")
	TSubclassOf<class UBulletDragVisualWidget> DragVisualClass = nullptr;

private:
	FName RowName = NAME_None;
	int32 PreDragCount = 0;
	int32 CurrentCount = 0;
};
