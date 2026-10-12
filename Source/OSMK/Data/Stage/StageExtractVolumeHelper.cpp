#include "Data/Stage/StageExtractVolumeHelper.h"
#include "Data/Stage/StageExtractedItem.h"
#include "GameFramework/Actor.h"
#include "Engine/Brush.h"
#include "Components/BrushComponent.h"
#include "Components/BoxComponent.h"

#if WITH_EDITOR
void StageExtractVolume::CaptureBrushBounds(const AActor* Actor, FStageExtractedItem& OutItem)
{
	const ABrush* Brush = Cast<ABrush>(Actor);
	if (!Brush)
	{
		return;
	}

	const UBrushComponent* BrushComp = Brush->GetBrushComponent();
	if (!BrushComp)
	{
		return;
	}

	const FBoxSphereBounds LocalBounds = BrushComp->CalcBounds(FTransform::Identity);
	if (LocalBounds.BoxExtent.IsNearlyZero())
	{
		return;
	}

	OutItem.bHasBrushBounds = true;
	OutItem.BrushBoxCenter = LocalBounds.Origin;
	OutItem.BrushBoxExtent = LocalBounds.BoxExtent;
	OutItem.BrushCollisionProfile = BrushComp->GetCollisionProfileName();
}
#endif

void StageExtractVolume::ApplyBrushBounds(AActor* SpawnedActor, const FStageExtractedItem& Item)
{
	if (!SpawnedActor || !Item.bHasBrushBounds || !Cast<ABrush>(SpawnedActor))
	{
		return;
	}

	USceneComponent* Root = SpawnedActor->GetRootComponent();
	if (!Root)
	{
		return;
	}

	UBoxComponent* Box = NewObject<UBoxComponent>(SpawnedActor, TEXT("BrushCollisionBox"));
	Box->SetMobility(Root->Mobility);
	Box->SetupAttachment(Root);
	Box->SetRelativeLocation(Item.BrushBoxCenter);
	Box->SetBoxExtent(Item.BrushBoxExtent, false);

	if (Item.BrushCollisionProfile != NAME_None)
	{
		Box->SetCollisionProfileName(Item.BrushCollisionProfile);
	}

	SpawnedActor->AddInstanceComponent(Box);
	Box->RegisterComponent();
}
