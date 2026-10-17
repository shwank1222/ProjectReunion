#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BulletDragVisualWidget.generated.h"

UCLASS()
class OSMK_API UBulletDragVisualWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitVisual(class UTexture2D* InIcon);
	void UpdateDrag(const FVector2D& CursorScreenPos);
	void SetGrabStart(const FVector2D& InStartTopLeft, const FVector2D& InEndPoint, const FVector2D& InStartSize, float InHeldScale, float InMoveDuration);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void TickSwing(float InDeltaTime, class UWidget* BulletRoot);
	class UWidget* GetBulletRoot() const;

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_BulletIcon = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UImage* Img_BulletPivot = nullptr;

	// UPROPERTY(EditDefaultsOnly, Category = "BulletAnim|Grab")
	// FVector2D HeldBulletSize = FVector2D(64.0f, 64.0f);

	// UPROPERTY(EditDefaultsOnly, Category = "BulletAnim|Grab", meta = (ClampMin = "0.0"))
	// float GrabMoveDuration = 0.12f;

	UPROPERTY(EditDefaultsOnly, Category = "DragVisual|Attach")
	float AttachAngle = 90.0f;

	UPROPERTY(EditDefaultsOnly, Category = "DragVisual|Attach", meta = (ClampMin = "0.0"))
	float AttachDuration = 0.15f;

	// UPROPERTY(EditDefaultsOnly, Category = "DragVisual|Attach")
	// FVector2D GrabPivot = FVector2D(0.0f, 0.5f);

	UPROPERTY(EditDefaultsOnly, Category = "DragVisual|Swing")
	float SwingSensitivity = 0.12f;

	UPROPERTY(EditDefaultsOnly, Category = "DragVisual|Swing", meta = (ClampMin = "0.0"))
	float SwingMaxAngle = 90.0f;

	UPROPERTY(EditDefaultsOnly, Category = "DragVisual|Swing", meta = (ClampMin = "0.0"))
	float SwingStiffness = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "DragVisual|Swing", meta = (ClampMin = "0.0"))
	float SwingDamping = 4.5f;

private:
	bool bHasCursorPos = false;
	FVector2D CurrentCursorPos = FVector2D::ZeroVector;
	bool bHasLastCursorPos = false;
	FVector2D LastCursorPos = FVector2D::ZeroVector;
	float AttachProgress = 0.0f;
	float SwingAngle = 0.0f;
	float SwingVelocity = 0.0f;

	FVector2D GrabStartTopLeft = FVector2D::ZeroVector;
	FVector2D GrabEndPoint = FVector2D::ZeroVector;
	FVector2D GrabStartSize = FVector2D::ZeroVector;
	float GrabProgress = 1.0f;
	float HeldBulletScale = 1.0f;
	float GrabMoveDuration = 0.12f;
};
