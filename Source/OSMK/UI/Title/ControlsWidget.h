#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ControlsWidget.generated.h"

class UButton;
class UCanvasPanel;

UCLASS()
class OSMK_API UControlsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION()
	void OnInGameClicked();

	UFUNCTION()
	void OnScoutClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_InGame = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Scout = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CP_InGame = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CP_Scout = nullptr;
};
