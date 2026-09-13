#include "StageDataExtractorWidget.h"
#include "Components/Button.h"
#include "Components/DetailsView.h"
#include "Data/StageData.h"
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
		if (!Settings->TargetStageData.IsNull())
		{
			TargetStageData = Settings->TargetStageData.LoadSynchronous();
		}
	}
}

void UStageDataExtractorWidget::OnExtractButtonClicked()
{
	UOSMKStageExtractorSettings* Settings = GetMutableDefault<UOSMKStageExtractorSettings>();
	if (Settings)
	{
		Settings->TargetLevels = TargetLevels;
		Settings->TargetStageData = TargetStageData;
		Settings->SaveConfig();
	}

	if (TargetStageData && TargetLevels.Num() > 0)
	{
		UStageDataExtractorLibrary::ExtractFromStageData(TargetStageData, TargetLevels);
	}
}
