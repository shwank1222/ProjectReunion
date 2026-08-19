#include "BulletDragVisualWidget.h"
#include "Components/Image.h"

void UBulletDragVisualWidget::InitVisual(UTexture2D* InIcon)
{
	if (Img_BulletIcon && InIcon)
	{
		Img_BulletIcon->SetBrushFromTexture(InIcon);
	}
}

void UBulletDragVisualWidget::UpdateDrag(const FVector2D& CursorScreenPos)
{
	if (!Img_BulletIcon)
	{
		return;
	}

	const UWorld* World = GetWorld();
	const float DeltaTime = World ? FMath::Min(World->GetDeltaSeconds(), 0.05f) : 0.0f;
	if (DeltaTime <= 0.0f)
	{
		LastCursorPos = CursorScreenPos;
		bHasLastCursorPos = true;
		return;
	}

	if (AttachDuration > 0.0f)
	{
		AttachProgress = FMath::Min(1.0f, AttachProgress + DeltaTime / AttachDuration);
	}
	else
	{
		AttachProgress = 1.0f;
	}
	const float AttachContribution = FMath::InterpEaseOut(0.0f, AttachAngle, AttachProgress, 2.0f);

	FVector2D CursorVelocity = FVector2D::ZeroVector;
	if (bHasLastCursorPos)
	{
		CursorVelocity = (CursorScreenPos - LastCursorPos) / DeltaTime;
	}
	LastCursorPos = CursorScreenPos;
	bHasLastCursorPos = true;

	const float TargetSwing = FMath::Clamp(CursorVelocity.X * SwingSensitivity, -SwingMaxAngle, SwingMaxAngle);
	const float SwingAccel = SwingStiffness * (TargetSwing - SwingAngle) - SwingDamping * SwingVelocity;
	SwingVelocity += SwingAccel * DeltaTime;
	SwingAngle += SwingVelocity * DeltaTime;
	SwingAngle = FMath::Clamp(SwingAngle, -SwingMaxAngle, SwingMaxAngle);

	Img_BulletIcon->SetRenderTransformPivot(GrabPivot);
	Img_BulletIcon->SetRenderTransformAngle(AttachContribution + SwingAngle);
}
