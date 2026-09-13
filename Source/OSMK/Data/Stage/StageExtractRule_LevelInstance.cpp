#include "Data/Stage/StageExtractRule_LevelInstance.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/World.h"
#include "LevelInstance/LevelInstanceActor.h"

UStageExtractRule_LevelInstance::UStageExtractRule_LevelInstance()
{
	TargetClass = ALevelInstance::StaticClass();
	Fields = static_cast<int32>(EStageExtractField::Transform) | static_cast<int32>(EStageExtractField::LevelAsset);
}

#if WITH_EDITOR
bool UStageExtractRule_LevelInstance::MatchesActor(const AActor* Actor) const
{
	return Cast<ALevelInstance>(Actor) != nullptr;
}

void UStageExtractRule_LevelInstance::FillItem(const AActor* Actor, FStageExtractedItem& OutItem) const
{
	Super::FillItem(Actor, OutItem);

	if (const ALevelInstance* LevelInst = Cast<ALevelInstance>(Actor))
	{
		if (HasField(EStageExtractField::LevelAsset))
		{
			OutItem.LevelAsset = LevelInst->GetWorldAsset();
		}
	}
}
#endif

void UStageExtractRule_LevelInstance::SpawnItem(FStageSpawnContext& Ctx, const FStageExtractedItem& Item) const
{
	if (!Ctx.World || Item.LevelAsset.IsNull())
	{
		return;
	}

	if (Ctx.BeginAsync)
	{
		Ctx.BeginAsync();
	}

	bool bSuccess = false;
	ULevelStreamingDynamic* Streaming = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
		Ctx.World,
		Item.LevelAsset,
		Item.Transform.GetLocation(),
		Item.Transform.GetRotation().Rotator(),
		bSuccess
	);

	if (bSuccess && Streaming)
	{
		if (Ctx.TrackStreaming)
		{
			Ctx.TrackStreaming(Streaming);
		}
	}
	else
	{
		if (Ctx.EndAsync)
		{
			Ctx.EndAsync();
		}
	}
}
