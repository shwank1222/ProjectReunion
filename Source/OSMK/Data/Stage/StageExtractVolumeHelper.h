#pragma once

#include "CoreMinimal.h"

class AActor;
struct FStageExtractedItem;

namespace StageExtractVolume
{
#if WITH_EDITOR
	void CaptureBrushBounds(const AActor* Actor, FStageExtractedItem& OutItem);
#endif

	void ApplyBrushBounds(AActor* SpawnedActor, const FStageExtractedItem& Item);
}
