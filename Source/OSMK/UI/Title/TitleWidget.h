#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleWidget.generated.h"

class UStageData;
class UButton;

UCLASS()
class OSMK_API UTitleWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnClickGameStart();

	UFUNCTION()
	void OnClickSettings();

	UFUNCTION()
	void OnClickCredits();

	UFUNCTION()
	void OnClickQuit();
	
	UFUNCTION()
	void OnClickControls();

	UFUNCTION()
	void OnQuitConfirmed();
	
protected:
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_GameStart = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Settings = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Controls = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Credits = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Quit = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
	UStageData* StageDataAsset = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> CreditsWidgetClass = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPopupConfirmWidget> PopupConfirmWidgetClass = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> SettingsWidgetClass = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> ControlsWidgetClass = nullptr;
};
