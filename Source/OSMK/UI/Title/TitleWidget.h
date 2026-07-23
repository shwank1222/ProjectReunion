#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleWidget.generated.h"

class UButton;

UCLASS()
class OSMK_API UTitleWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION()
	void OnClickGameStart();

	UFUNCTION()
	void OnClickSettings();

	UFUNCTION()
	void OnClickCredits();

	UFUNCTION()
	void OnClickQuit();
	
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_GameStart = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Settings = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Quit = nullptr;
};
