#include "StageDataExtractorLibrary.h"
#include "Engine/DataTable.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Data/Stage/StageStaticMeshData.h"

#if WITH_EDITOR
#include "EngineUtils.h"
#endif

void UStageDataExtractorLibrary::ExtractStaticMeshFromLevels(UDataTable* TargetDataTable, TArray<TSoftObjectPtr<UWorld>> TargetLevels)
{
#if WITH_EDITOR
	if (!TargetDataTable)
	{
		return;
	}

	TargetDataTable->EmptyTable();

	for (const TSoftObjectPtr<UWorld>& SoftWorld : TargetLevels)
	{
		if (SoftWorld.IsNull())
		{
			continue;
		}

		UWorld* LoadedWorld = SoftWorld.LoadSynchronous();
		if (!LoadedWorld)
		{
			continue;
		}

		FName RowName = FName(*LoadedWorld->GetName());
		FStageStaticMeshData NewStageData;

		for (TActorIterator<AStaticMeshActor> It(LoadedWorld); It; ++It)
		{
			AStaticMeshActor* SMActor = *It;
			if (!SMActor)
			{
				continue;
			}

			UStaticMeshComponent* SMComp = SMActor->GetStaticMeshComponent();
			if (!SMComp || !SMComp->GetStaticMesh())
			{
				continue;
			}

			FStageStaticMeshItem ItemData;
			ItemData.StaticMesh = TSoftObjectPtr<UStaticMesh>(SMComp->GetStaticMesh());
			ItemData.Transform = SMActor->GetActorTransform();
			ItemData.CollisionProfileName = SMComp->GetCollisionProfileName();

			NewStageData.MeshList.Add(ItemData);
		}

		TargetDataTable->AddRow(RowName, NewStageData);
	}

	TargetDataTable->MarkPackageDirty();
#endif
}