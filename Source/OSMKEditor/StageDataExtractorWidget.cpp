#include "StageDataExtractorWidget.h"
#include "Components/Button.h"
#include "Components/DetailsView.h"
#include "Utility/Stage/StageDataExtractorLibrary.h"

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
}

void UStageDataExtractorWidget::OnExtractButtonClicked()
{
	if (TargetLevels.Num() > 0)
	{
		if (StaticMeshDataTable)
		{
			UStageDataExtractorLibrary::ExtractStaticMeshFromLevels(StaticMeshDataTable, TargetLevels);
		}
	}
}