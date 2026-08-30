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

enum class EBulletFlyMode : uint8
{
	LoadToCylinder,
	PopFromCylinder,
	ReturnToItem,
};

UCLASS()
class OSMK_API UBulletSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
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
	void RefreshConfirmButton();
	void UpdateSlotImages();

	void OnBulletHovered(FName RowName);
	void OnBulletUnhovered(FName RowName);

	void HandleBulletDropped(FName RowName, const FVector2D& CursorScreenPos, bool bOnCylinder);
	void StartCursorFly(FName BulletName, const FVector2D& CursorScreenPos, int32 SlotIndex, EBulletFlyMode Mode);
	void StartPopFly(int32 SlotIndex);
	void SetItemIconHidden(FName RowName, bool bHidden);
	void SetInteractable(bool bEnabled);
	void FinishReset();

	FVector2D LocalPositionOf(const class UWidget* Widget) const;

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
	class UOverlay* Canvas_Cylinder = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* CylinderPanel = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	class UCanvasPanel* Canvas_FlyOverlay = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* BulletDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UBulletListItemWidget> BulletItemWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Fly", meta = (ClampMin = "0.01"))
	float FlySpeed = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Fly")
	float FlyingBulletUprightAngle = 90.0f;

	UPROPERTY(EditDefaultsOnly, Category = "ConnectionLine", meta = (ClampMin = "0.1"))
	float LineDrawSpeed = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "ConnectionLine")
	bool bLoopLine = false;

	UPROPERTY(EditDefaultsOnly, Category = "ConnectionLine")
	FLinearColor LineColor = FLinearColor(1.f, 1.f, 1.f, 0.85f);

	UPROPERTY(EditDefaultsOnly, Category = "ConnectionLine", meta = (ClampMin = "0.1"))
	float LineThickness = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "ConnectionLine", meta = (ClampMin = "1.0"))
	float DashLength = 12.f;

	UPROPERTY(EditDefaultsOnly, Category = "ConnectionLine", meta = (ClampMin = "0.0"))
	float DashGap = 6.f;

	UPROPERTY(EditDefaultsOnly, Category = "ConnectionLine", meta = (ClampMin = "1.0"))
	float ArrowSize = 8.f;

private:
	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* Img_FlyingBullet = nullptr;
	
	UPROPERTY()
	TArray<class UImage*> ResetFlyImages;
	
	TArray<FResetFlyEntry> ResetFlyEntries;
	TArray<FName> SlotBullets;
	
	TMap<FName, class UBulletListItemWidget*> ListItemWidgets;

	bool bIsDrawingLine = false;
	float DrawProgress = 0.0f;
	float LoopDelayRemaining = 0.0f;
	FVector2D LineStartLocal = FVector2D::ZeroVector;
	FVector2D LineEndLocal = FVector2D::ZeroVector;

	float CurrentRotationAngle = 0.0f;
	float TargetRotationAngle  = 0.0f;
	float FlyProgress     = 0.0f;
	float FlyAngle        = 0.0f;
	
	bool  bIsFlying       = false;
	
	EBulletFlyMode FlyMode = EBulletFlyMode::LoadToCylinder;
	
	FName PendingBulletName;
	
	int32 PendingSlotIndex  = -1;
	
	FVector2D FlyStartPos   = FVector2D::ZeroVector;
	FVector2D FlyEndPos     = FVector2D::ZeroVector;
	FVector2D FlyStartSize  = FVector2D::ZeroVector;
	FVector2D FlyEndSize    = FVector2D::ZeroVector;
	FVector2D FlyTextureSize = FVector2D::ZeroVector;
};
