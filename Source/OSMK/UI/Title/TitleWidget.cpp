#include "UI/Title/TitleWidget.h"
#include "Components/Button.h"

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