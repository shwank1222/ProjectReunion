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

private:
	TWeakObjectPtr<APlayerCharacter> CachedPlayerCharacter = nullptr;
};
