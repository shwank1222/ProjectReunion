#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoutingWidget.generated.h"

UCLASS()
class OSMK_API UScoutingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void OnConfirmClicked();
	
	void PopulateBulletList();
	void InitSlots();
	void AddBulletToSlot(FName RowName);
	void RemoveBulletFromSlot(int32 SlotIndex);
	void RefreshConfirmButton();
	
public:
	static constexpr int32 MaxBulletSlots = 6;
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* List_Bullets = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* Box_Slots = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Confirm = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Scouting")
	class UDataTable* BulletDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Scouting")
	TSubclassOf<class UBulletListItemWidget> BulletItemWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Scouting")
	TSubclassOf<class UBulletSlotWidget> SlotWidgetClass = nullptr;

private:
	TArray<FName> SlotBullets;
	TArray<class UBulletSlotWidget*> SlotWidgets;
};
