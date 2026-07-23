#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BulletSlotWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnSlotClicked, int32);

UCLASS()
class OSMK_API UBulletSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(int32 InSlotIndex);
	void SetBullet(FName InRowName, UTexture2D* InIcon);
	void ClearSlot();
	bool IsEmpty() const { return CurrentRowName.IsNone(); }
	
	FOnSlotClicked OnSlotClicked;

protected:
	virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void OnSlotButtonClicked();
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Slot = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Icon = nullptr;

private:
	int32 SlotIndex = -1;
	FName CurrentRowName = NAME_None;
};
