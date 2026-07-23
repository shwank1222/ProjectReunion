#include "UI/Title/TitleWidget.h"
#include "Components/Button.h"
#include "Data/StageData.h"
#include "Kismet/GameplayStatics.h"

void UTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_GameStart)
	{
		Btn_GameStart->OnClicked.AddDynamic(this, &UTitleWidget::OnClickGameStart);
	}
	if (Btn_Settings)
	{
		Btn_Settings->OnClicked.AddDynamic(this, &UTitleWidget::OnClickSettings);
	}
	if (Btn_Quit)
	{
		Btn_Quit->OnClicked.AddDynamic(this, &UTitleWidget::OnClickQuit);
	}
}

void UTitleWidget::OnClickGameStart()
{
	if (StageDataAsset)
	{
		if (!StageDataAsset->InGameLevel.IsNull())
		{
			UGameplayStatics::OpenLevelBySoftObjectPtr(this, StageDataAsset->InGameLevel);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[StageData] Null StageLevel"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[TitleWidget] Null StageData"));
	}
}

void UTitleWidget::OnClickSettings()
{
}

void UTitleWidget::OnClickCredits()
{
}

void UTitleWidget::OnClickQuit()
{
}