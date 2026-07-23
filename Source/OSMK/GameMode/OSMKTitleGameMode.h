#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "OSMKTitleGameMode.generated.h"


UCLASS()
class OSMK_API AOSMKTitleGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> TitleWidgetClass = nullptr;
};
