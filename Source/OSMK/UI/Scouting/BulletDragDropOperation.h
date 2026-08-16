#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "BulletDragDropOperation.generated.h"

UCLASS()
class OSMK_API UBulletDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	virtual void Dragged_Implementation(const FPointerEvent& PointerEvent) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop")
	FName BulletRowName = NAME_None;
};
