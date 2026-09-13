#include "Data/Stage/StageExtractRule_PlayerStart.h"
#include "GameFramework/PlayerStart.h"

UStageExtractRule_PlayerStart::UStageExtractRule_PlayerStart()
{
	TargetClass = APlayerStart::StaticClass();
	Fields = static_cast<int32>(EStageExtractField::Transform);
}

void UStageExtractRule_PlayerStart::SpawnItem(FStageSpawnContext& Ctx, const FStageExtractedItem& Item) const
{
	if (Ctx.SetPlayerStart)
	{
		Ctx.SetPlayerStart(Item.Transform);
	}
}
