#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreditsWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnCreditsFinished);

UCLASS()
class OSMK_API UCreditsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
private:
	void CloseCredits();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credits")
	bool bFastForwardOnInput = false;

	FOnCreditsFinished OnCreditsFinished;
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* Box_Credits = nullptr;

	UPROPERTY(EditAnywhere, Category = "Credits")
	float ScrollSpeed = 150.0f;

private:
	float CurrentY = 0.0f;
	float EndPositionY = 0.0f;
	float OriginalScrollSpeed = 0.0f;
	float CurrentOpacity = 0.0f;
	bool bIsClosing = false;
	bool bEndPositionReady = false;
};