#include "BulletDragVisualWidget.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"

void UBulletDragVisualWidget::InitVisual(UTexture2D* InIcon)
{
	if (Img_BulletIcon && InIcon)
	{
		Img_BulletIcon->SetBrushFromTexture(InIcon);
	}

	// if (Img_BulletIcon)
	// {
	// 	Img_BulletIcon->SetRenderTransformPivot(GrabPivot);
	// 	Img_BulletIcon->SetRenderOpacity(0.0f);
	// }

	if (UWidget* BulletRoot = GetBulletRoot())
	{
		BulletRoot->SetRenderOpacity(0.0f);
	}

	if (Img_BulletPivot)
	{
		Img_BulletPivot->SetRenderOpacity(0.0f);
	}
}

UWidget* UBulletDragVisualWidget::GetBulletRoot() const
{
	return Img_BulletIcon ? Img_BulletIcon->GetParent() : nullptr;
}

void UBulletDragVisualWidget::SetGrabStart(const FVector2D& InStartTopLeft, const FVector2D& InEndPoint, const FVector2D& InStartSize, float InHeldScale, float InMoveDuration)
{
	GrabStartTopLeft = InStartTopLeft;
	GrabEndPoint = InEndPoint;
	GrabStartSize = InStartSize;
	HeldBulletScale = InHeldScale;
	GrabMoveDuration = InMoveDuration;
	GrabProgress = 0.0f;
}

void UBulletDragVisualWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UWidget* BulletRoot = GetBulletRoot();
	if (!BulletRoot || !Img_BulletPivot)
	{
		return;
	}

	const FGeometry& RootGeometry = BulletRoot->GetCachedGeometry();
	const FGeometry& IconGeometry = Img_BulletIcon->GetCachedGeometry();
	const FGeometry& PivotGeometry = Img_BulletPivot->GetCachedGeometry();
	const FVector2D RootSize = RootGeometry.GetLocalSize();
	const FVector2D IconSize = IconGeometry.GetLocalSize();
	if (RootSize.X <= KINDA_SMALL_NUMBER || RootSize.Y <= KINDA_SMALL_NUMBER || IconSize.X <= KINDA_SMALL_NUMBER || IconSize.Y <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	const float LayoutScaleRatio = RootGeometry.GetAccumulatedLayoutTransform().GetScale() / MyGeometry.GetAccumulatedLayoutTransform().GetScale();
	if (LayoutScaleRatio <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	const FVector2D PivotAbs = FVector2D(PivotGeometry.LocalToAbsolute(PivotGeometry.GetLocalSize() * 0.5f));
	const FVector2D PivotLocal = FVector2D(RootGeometry.AbsoluteToLocal(PivotAbs));
	const FVector2D PivotInIcon = FVector2D(IconGeometry.AbsoluteToLocal(PivotAbs)) / IconSize;

	if (GrabProgress < 1.0f)
	{
		GrabProgress = GrabMoveDuration > 0.0f ? FMath::Min(1.0f, GrabProgress + InDeltaTime / GrabMoveDuration) : 1.0f;
	}

	const float Alpha = FMath::InterpEaseOut(0.0f, 1.0f, GrabProgress, 2.0f);
	const FVector2D StartScale = GrabStartSize / (IconSize * LayoutScaleRatio);
	const FVector2D CurrentScale = FMath::Lerp(StartScale, FVector2D(HeldBulletScale, HeldBulletScale), Alpha);
	const FVector2D StartPoint = GrabStartTopLeft + GrabStartSize * PivotInIcon;
	const FVector2D CurrentPoint = FMath::Lerp(StartPoint, GrabEndPoint, Alpha);

	const FVector2f OldPivotPx = FVector2f(RootSize * BulletRoot->GetRenderTransformPivot());
	const FSlateRenderTransform OldTransform = Concatenate(Inverse(OldPivotPx), BulletRoot->GetRenderTransform().ToSlateRenderTransform(), OldPivotPx);
	const FVector2D CursorInRoot = FVector2D(RootGeometry.AbsoluteToLocal(MyGeometry.LocalToAbsolute(CurrentPoint)));
	const FVector2D RestCursor = FVector2D(TransformPoint(OldTransform, FVector2f(CursorInRoot)));

	BulletRoot->SetRenderTransformPivot(PivotLocal / RootSize);
	BulletRoot->SetRenderScale(CurrentScale);
	BulletRoot->SetRenderTranslation(RestCursor - PivotLocal);
	BulletRoot->SetRenderOpacity(1.0f);

	TickSwing(InDeltaTime, BulletRoot);

	// const FGeometry& IconGeometry = Img_BulletIcon->GetCachedGeometry();
	// const FVector2D BoxSize = IconGeometry.GetLocalSize();
	// if (BoxSize.X <= KINDA_SMALL_NUMBER || BoxSize.Y <= KINDA_SMALL_NUMBER)
	// {
	// 	return;
	// }
	//
	// if (GrabProgress < 1.0f)
	// {
	// 	GrabProgress = GrabMoveDuration > 0.0f ? FMath::Min(1.0f, GrabProgress + InDeltaTime / GrabMoveDuration) : 1.0f;
	// }
	//
	// const float Alpha = FMath::InterpEaseOut(0.0f, 1.0f, GrabProgress, 2.0f);
	// const FVector2D CurrentSize = FMath::Lerp(GrabStartSize, HeldBulletSize, Alpha);
	// const FVector2D CurrentScale = FMath::Lerp(GrabStartSize / BoxSize, FVector2D(HeldBulletScale, HeldBulletScale), Alpha);
	// const FVector2D CurrentPoint = FMath::Lerp(GrabStartPoint, GrabEndPoint, Alpha);
	// const FVector2D IconOrigin = MyGeometry.AbsoluteToLocal(IconGeometry.GetAbsolutePosition());
	// const FVector2D RestGrabPoint = IconOrigin + BoxSize * GrabPivot;
	// const FVector2D PivotPoint = MyGeometry.AbsoluteToLocal(IconGeometry.LocalToAbsolute(BoxSize * GrabPivot));
	// const FVector2D RestGrabPoint = PivotPoint - Img_BulletIcon->GetRenderTransform().Translation;
	//
	// Img_BulletIcon->SetRenderScale(CurrentSize / BoxSize);
	// Img_BulletIcon->SetRenderScale(CurrentScale);
	// Img_BulletIcon->SetRenderTranslation(CurrentPoint - RestGrabPoint);
	// Img_BulletIcon->SetRenderOpacity(1.0f);
	//
	// TickSwing(InDeltaTime);
}

void UBulletDragVisualWidget::TickSwing(float InDeltaTime, UWidget* BulletRoot)
{
	const float DeltaTime = FMath::Min(InDeltaTime, 0.05f);
	if (DeltaTime <= 0.0f || !bHasCursorPos)
	{
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
		CursorVelocity = (CurrentCursorPos - LastCursorPos) / DeltaTime;
	}
	LastCursorPos = CurrentCursorPos;
	bHasLastCursorPos = true;

	const float TargetSwing = FMath::Clamp(CursorVelocity.X * SwingSensitivity, -SwingMaxAngle, SwingMaxAngle);
	const float SwingAccel = SwingStiffness * (TargetSwing - SwingAngle) - SwingDamping * SwingVelocity;
	SwingVelocity += SwingAccel * DeltaTime;
	SwingAngle += SwingVelocity * DeltaTime;
	SwingAngle = FMath::Clamp(SwingAngle, -SwingMaxAngle, SwingMaxAngle);

	// Img_BulletIcon->SetRenderTransformPivot(GrabPivot);
	// Img_BulletIcon->SetRenderTransformAngle(AttachContribution + SwingAngle);
	BulletRoot->SetRenderTransformAngle(AttachContribution + SwingAngle);
}

void UBulletDragVisualWidget::UpdateDrag(const FVector2D& CursorScreenPos)
{
	CurrentCursorPos = CursorScreenPos;
	bHasCursorPos = true;

	// if (!Img_BulletIcon)
	// {
	// 	return;
	// }
	//
	// const UWorld* World = GetWorld();
	// const float DeltaTime = World ? FMath::Min(World->GetDeltaSeconds(), 0.05f) : 0.0f;
	// if (DeltaTime <= 0.0f)
	// {
	// 	LastCursorPos = CursorScreenPos;
	// 	bHasLastCursorPos = true;
	// 	return;
	// }
	//
	// if (AttachDuration > 0.0f)
	// {
	// 	AttachProgress = FMath::Min(1.0f, AttachProgress + DeltaTime / AttachDuration);
	// }
	// else
	// {
	// 	AttachProgress = 1.0f;
	// }
	// const float AttachContribution = FMath::InterpEaseOut(0.0f, AttachAngle, AttachProgress, 2.0f);
	//
	// FVector2D CursorVelocity = FVector2D::ZeroVector;
	// if (bHasLastCursorPos)
	// {
	// 	CursorVelocity = (CursorScreenPos - LastCursorPos) / DeltaTime;
	// }
	// LastCursorPos = CursorScreenPos;
	// bHasLastCursorPos = true;
	//
	// const float TargetSwing = FMath::Clamp(CursorVelocity.X * SwingSensitivity, -SwingMaxAngle, SwingMaxAngle);
	// const float SwingAccel = SwingStiffness * (TargetSwing - SwingAngle) - SwingDamping * SwingVelocity;
	// SwingVelocity += SwingAccel * DeltaTime;
	// SwingAngle += SwingVelocity * DeltaTime;
	// SwingAngle = FMath::Clamp(SwingAngle, -SwingMaxAngle, SwingMaxAngle);
	//
	// Img_BulletIcon->SetRenderTransformPivot(GrabPivot);
	// Img_BulletIcon->SetRenderTransformAngle(AttachContribution + SwingAngle);
}
