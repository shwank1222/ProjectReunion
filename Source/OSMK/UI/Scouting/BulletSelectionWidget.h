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
	void UpdateListItemCount(FName RowName);
	
private:
	UFUNCTION()
	void OnConfirmClicked();
	
	UFUNCTION()
	void OnResetClicked();
	
	UFUNCTION()
	void OnPopClicked();
	
	void PopulateBulletList();
	void AddBulletToSlot(FName RowName);
	void RefreshConfirmButton();
	void UpdateSlotImages();

private:
	int32 GetAvailableCount(FName RowName) const;
	int32 GetSelectedCount(FName RowName) const;
	
public:
	static constexpr int32 MaxBulletSlots = AOSMKGameState::MaxBulletSlots;
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* List_Bullets = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> BulletSlot_1 = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> BulletSlot_2 = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> BulletSlot_3 = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> BulletSlot_4 = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> BulletSlot_5 = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> BulletSlot_6 = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Confirm = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Reset = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Pop = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* BulletDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UBulletListItemWidget> BulletItemWidgetClass = nullptr;

private:
	TArray<FName> SlotBullets;
	TMap<FName, class UBulletListItemWidget*> ListItemWidgets;
};
