#include "GameMode/OSMKTitleGameMode.h"
#include "Blueprint/UserWidget.h"

void AOSMKTitleGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (TitleWidgetClass)
	{
		UUserWidget* TitleWidget = CreateWidget<UUserWidget>(GetWorld(), TitleWidgetClass);
		if (TitleWidget)
		{
			TitleWidget->AddToViewport();
		}
	}
}
