#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngameBulletSlotWidget.generated.h"

class APlayerCharacter;
class UImage;

UCLASS()
class OSMK_API UIngameBulletSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void UpdateBullets();

private:
	UFUNCTION()
	void OnPawnChanged(APawn* OldPawn, APawn* NewPawn);

	void TryBindToPlayerCharacter();
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BulletSlot_1 = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BulletSlot_2 = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BulletSlot_3 = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BulletSlot_4 = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BulletSlot_5 = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BulletSlot_6 = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* CylinderPanel = nullptr;
	
private:
	TWeakObjectPtr<APlayerCharacter> CachedPlayerCharacter = nullptr;
	float CurrentRotationAngle = 0.0f;
	float TargetRotationAngle = 0.0f;
	int32 PreviousAmmoCount = 0;
};
