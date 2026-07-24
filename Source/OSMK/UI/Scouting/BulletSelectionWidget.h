#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/OSMKGameState.h"
#include "BulletSelectionWidget.generated.h"

UCLASS()
class OSMK_API UBulletSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void OnConfirmClicked();
	
	UFUNCTION()
	void OnResetClicked();
	
	void PopulateBulletList();
	void InitSlots();
	void AddBulletToSlot(FName RowName);
	void RemoveBulletFromSlot(int32 SlotIndex);
	void RefreshConfirmButton();
	
public:
	static constexpr int32 MaxBulletSlots = AOSMKGameState::MaxBulletSlots;
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* List_Bullets = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* Box_Slots = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Confirm = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Reset = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* BulletDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UBulletListItemWidget> BulletItemWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UBulletSlotWidget> SlotWidgetClass = nullptr;

private:
	TArray<FName> SlotBullets;
	TArray<class UBulletSlotWidget*> SlotWidgets;
};
