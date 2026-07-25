#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BulletCountdownWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;
class APlayerCharacter;

UCLASS()
class OSMK_API UBulletCountdownWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnPawnChanged(APawn* OldPawn, APawn* NewPawn);

	void TryBindToPlayerCharacter();

	UFUNCTION()
	void HandleAmmoChanged();
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Countdown = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_ZoomFade = nullptr;

	TWeakObjectPtr<APlayerCharacter> CachedPlayerCharacter = nullptr;
	int32 PreviousAmmoCount = -1;
};
