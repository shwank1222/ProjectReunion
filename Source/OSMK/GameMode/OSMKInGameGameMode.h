#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "OSMKInGameGameMode.generated.h"

UCLASS()
class OSMK_API AOSMKInGameGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UScoutingWidget> ScoutingWidgetClass = nullptr;

private:
	UPROPERTY()
	class UScoutingWidget* ScoutingWidget = nullptr;
};
