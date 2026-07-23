#include "GameMode/OSMKInGameGameMode.h"
#include "UI/Scouting/ScoutingWidget.h"
#include "Blueprint/UserWidget.h"

void AOSMKInGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!ScoutingWidgetClass)
	{
		return;
	}

	ScoutingWidget = CreateWidget<UScoutingWidget>(GetWorld(), ScoutingWidgetClass);
	if (ScoutingWidget)
	{
		ScoutingWidget->AddToViewport();
	}
}
