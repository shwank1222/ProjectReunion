#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoutingWidget.generated.h"

UCLASS()
class OSMK_API UScoutingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnBulletPrepClicked();

	UFUNCTION()
	void OnScoutClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	class UBulletSelectionWidget* BulletSelectionWidget = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_BulletPrep = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Scout = nullptr;
};
