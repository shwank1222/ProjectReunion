#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OSMKIngameHUD.generated.h"

class UBulletCountdownWidget;
class UIngameBulletSlotWidget;

UCLASS()
class OSMK_API AOSMKIngameHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void SetHUDVisible(bool bVisible);

private:
	UFUNCTION()
	void OnPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UIngameBulletSlotWidget> BulletSlotWidgetClass = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBulletCountdownWidget> BulletCountdownWidgetClass = nullptr;

private:
	UPROPERTY()
	TObjectPtr<UIngameBulletSlotWidget> BulletSlotWidgetInstance = nullptr;
	
	UPROPERTY()
	TObjectPtr<UBulletCountdownWidget> BulletCountdownWidgetInstance = nullptr;
};
