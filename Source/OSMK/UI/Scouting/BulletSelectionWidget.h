#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/OSMKGameState.h"
#include "BulletSelectionWidget.generated.h"

struct FResetFlyEntry
{
	class UImage* Image = nullptr;
	FVector2D StartPos = FVector2D::ZeroVector;
	FVector2D EndPos = FVector2D::ZeroVector;
	FVector2D StartSize = FVector2D::ZeroVector;
	FVector2D EndSize = FVector2D::ZeroVector;
	float Progress = 0.0f;
};

UCLASS()
class OSMK_API UBulletSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void UpdateListItemCount(FName RowName);

private:
	UFUNCTION()
	void OnConfirmClicked();

	UFUNCTION()
	void OnResetClicked();

	UFUNCTION()
	void OnResetHovered();

	UFUNCTION()
	void OnResetUnhovered();

	UFUNCTION()
	void OnPopClicked();

	UFUNCTION()
	void OnPopHovered();

	UFUNCTION()
	void OnPopUnhovered();

	void PopulateBulletList();
	void AddBulletToSlot(FName RowName);
	void RefreshConfirmButton();
	void UpdateSlotImages();
	void StartFlyAnimation(FName BulletName, int32 SlotIndex, bool bToSlot);
	void SetInteractable(bool bEnabled);
	void FinishReset();

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
	class UImage* Img_Action = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Action = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_Confirm = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Reset = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Pop = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* CylinderPanel = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	class UCanvasPanel* Canvas_FlyOverlay = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* BulletDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UBulletListItemWidget> BulletItemWidgetClass = nullptr;

private:
	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* Img_FlyingBullet = nullptr;
	
	UPROPERTY()
	TArray<class UImage*> ResetFlyImages;
	
	TArray<FName> SlotBullets;
	TMap<FName, class UBulletListItemWidget*> ListItemWidgets;
	float CurrentRotationAngle = 0.0f;
	float TargetRotationAngle  = 0.0f;
	bool  bIsFlying       = false;
	bool  bIsFlyingToSlot = true;
	float FlyProgress     = 0.0f;
	FName PendingBulletName;
	int32 PendingSlotIndex  = -1;
	FVector2D FlyStartPos   = FVector2D::ZeroVector;
	FVector2D FlyEndPos     = FVector2D::ZeroVector;
	FVector2D FlyStartSize  = FVector2D::ZeroVector;
	FVector2D FlyEndSize    = FVector2D::ZeroVector;
	TArray<FResetFlyEntry> ResetFlyEntries;
};
