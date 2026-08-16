#include "BulletDragDropOperation.h"
#include "BulletDragVisualWidget.h"
#include "BulletListItemWidget.h"

void UBulletDragDropOperation::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Dragged_Implementation(PointerEvent);

	if (UBulletDragVisualWidget* Visual = Cast<UBulletDragVisualWidget>(DefaultDragVisual))
	{
		Visual->UpdateDrag(PointerEvent.GetScreenSpacePosition());
	}
}

void UBulletDragDropOperation::DragCancelled_Implementation(const FPointerEvent& PointerEvent)
{
	Super::DragCancelled_Implementation(PointerEvent);

	if (SourceItem.IsValid())
	{
		SourceItem->RestoreFromDrag();
	}
}
