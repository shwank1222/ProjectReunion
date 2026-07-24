#include "UI/StageResult/StageFailWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/OSMKInGameGameMode.h"

void UStageFailWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Retry)
	{
		Btn_Retry->OnClicked.AddDynamic(this, &UStageFailWidget::OnRetryClicked);
	}

	if (Btn_Title)
	{
		Btn_Title->OnClicked.AddDynamic(this, &UStageFailWidget::OnTitleClicked);
	}
}

void UStageFailWidget::OnRetryClicked()
{
	RemoveFromParent();

	if (AOSMKInGameGameMode* GM = Cast<AOSMKInGameGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->RetryStage();
	}
}

void UStageFailWidget::OnTitleClicked()
{
	UGameplayStatics::OpenLevel(this, FName("L_Title"));
}