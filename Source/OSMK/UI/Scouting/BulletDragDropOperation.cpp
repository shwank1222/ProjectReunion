#include "BulletDragDropOperation.h"
#include "BulletDragVisualWidget.h"

void UBulletDragDropOperation::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Dragged_Implementation(PointerEvent);

	if (UBulletDragVisualWidget* Visual = Cast<UBulletDragVisualWidget>(DefaultDragVisual))
	{
		Visual->UpdateDrag(PointerEvent.GetScreenSpacePosition());
	}
}
