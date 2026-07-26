#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OSMKIngameHUD.generated.h"

class UKillFeedbackListWidget;
class UBulletCountdownWidget;
class UIngameBulletSlotWidget;

UCLASS()
class OSMK_API AOSMKIngameHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void SetHUDVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowKillFeedback();
	
private:
	UFUNCTION()
	void OnPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UFUNCTION()
	void HandleEnemyCountChanged();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UIngameBulletSlotWidget> BulletSlotWidgetClass = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBulletCountdownWidget> BulletCountdownWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UKillFeedbackListWidget> KillFeedbackListWidgetClass = nullptr;

private:
	UPROPERTY()
	TObjectPtr<UIngameBulletSlotWidget> BulletSlotWidgetInstance = nullptr;
	
	UPROPERTY()
	TObjectPtr<UBulletCountdownWidget> BulletCountdownWidgetInstance = nullptr;

	UPROPERTY()
	TObjectPtr<UKillFeedbackListWidget> KillFeedbackListWidgetInstance = nullptr;
	
	int32 CachedEnemyCount = 0;
};
