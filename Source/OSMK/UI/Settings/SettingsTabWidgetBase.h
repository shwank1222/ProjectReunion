#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsTabWidgetBase.generated.h"

UCLASS(Abstract)
class OSMK_API USettingsTabWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnOpenTab(class UOSMKSaveGame* InSaveGame) {}

	virtual void NativeOnConfirmSettings(class UOSMKSaveGame* OutSaveGame) {}

	virtual void NativeOnCancelSettings() {}

	virtual void NativeOnResetToDefault() {}
};