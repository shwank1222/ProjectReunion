#include "StageDataExtractorWidget.h"
#include "Components/Button.h"
#include "Components/DetailsView.h"
#include "Utility/Stage/StageDataExtractorLibrary.h"
#include "OSMKStageExtractorSettings.h"

void UStageDataExtractorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ExtractButton)
	{
		ExtractButton->OnClicked.AddDynamic(this, &UStageDataExtractorWidget::OnExtractButtonClicked);
	}

	if (LevelDetailsView)
	{
		LevelDetailsView->CategoriesToShow.Add(FName("Stage Data Extractor"));
		LevelDetailsView->SetObject(this);
	}

	const UOSMKStageExtractorSettings* Settings = GetDefault<UOSMKStageExtractorSettings>();
	if (Settings)
	{
		TargetLevels = Settings->TargetLevels;
	}
}

void UStageDataExtractorWidget::OnExtractButtonClicked()
{
	UOSMKStageExtractorSettings* Settings = GetMutableDefault<UOSMKStageExtractorSettings>();
	if (Settings)
	{
		Settings->TargetLevels = TargetLevels;
		Settings->SaveConfig();
	}

	if (TargetLevels.Num() > 0 && StaticMeshDataTable)
	{
		UStageDataExtractorLibrary::ExtractStaticMeshFromLevels(StaticMeshDataTable, TargetLevels);
	}
}
