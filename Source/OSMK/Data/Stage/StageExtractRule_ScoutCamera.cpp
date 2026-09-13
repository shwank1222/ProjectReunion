#include "Data/Stage/StageExtractRule_ScoutCamera.h"
#include "GameFramework/SpringArmComponent.h"

UStageExtractRule_ScoutCamera::UStageExtractRule_ScoutCamera()
{
	TargetClass = AActor::StaticClass();
	Fields = static_cast<int32>(EStageExtractField::Class) |
	         static_cast<int32>(EStageExtractField::Transform) |
	         static_cast<int32>(EStageExtractField::SpringArm);
}

#if WITH_EDITOR
bool UStageExtractRule_ScoutCamera::MatchesActor(const AActor* Actor) const
{
	return Actor && Actor->FindComponentByClass<USpringArmComponent>() != nullptr;
}

void UStageExtractRule_ScoutCamera::FillItem(const AActor* Actor, FStageExtractedItem& OutItem) const
{
	Super::FillItem(Actor, OutItem);

	if (Actor)
	{
		if (const USpringArmComponent* SpringArm = Actor->FindComponentByClass<USpringArmComponent>())
		{
			if (HasField(EStageExtractField::SpringArm))
			{
				OutItem.SpringArmLength = SpringArm->TargetArmLength;
				OutItem.SpringArmSocketOffset = SpringArm->SocketOffset;
			}
		}
	}
}
#endif

void UStageExtractRule_ScoutCamera::ConfigureSpawnedActor(AActor* SpawnedActor, const FStageExtractedItem& Item, FStageSpawnContext& Ctx) const
{
	Super::ConfigureSpawnedActor(SpawnedActor, Item, Ctx);

	if (!SpawnedActor)
	{
		return;
	}

	if (HasField(EStageExtractField::SpringArm))
	{
		if (USpringArmComponent* SpringArm = SpawnedActor->FindComponentByClass<USpringArmComponent>())
		{
			SpringArm->TargetArmLength = Item.SpringArmLength;
			SpringArm->SocketOffset = Item.SpringArmSocketOffset;
		}
	}

	if (Ctx.SetScoutCamera)
	{
		Ctx.SetScoutCamera(SpawnedActor);
	}
}
