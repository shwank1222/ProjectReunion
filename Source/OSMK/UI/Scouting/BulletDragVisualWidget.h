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

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_BulletIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "DragVisual|Attach")
	float AttachAngle = 90.0f;

	UPROPERTY(EditDefaultsOnly, Category = "DragVisual|Attach", meta = (ClampMin = "0.0"))
	float AttachDuration = 0.15f;

	UPROPERTY(EditDefaultsOnly, Category = "DragVisual|Attach")
	FVector2D GrabPivot = FVector2D(0.0f, 0.5f);

	UPROPERTY(EditDefaultsOnly, Category = "DragVisual|Swing")
	float SwingSensitivity = 0.12f;

	UPROPERTY(EditDefaultsOnly, Category = "DragVisual|Swing", meta = (ClampMin = "0.0"))
	float SwingMaxAngle = 90.0f;

	UPROPERTY(EditDefaultsOnly, Category = "DragVisual|Swing", meta = (ClampMin = "0.0"))
	float SwingStiffness = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "DragVisual|Swing", meta = (ClampMin = "0.0"))
	float SwingDamping = 4.5f;

private:
	bool bHasLastCursorPos = false;
	FVector2D LastCursorPos = FVector2D::ZeroVector;
	float AttachProgress = 0.0f;
	float SwingAngle = 0.0f;
	float SwingVelocity = 0.0f;
};
