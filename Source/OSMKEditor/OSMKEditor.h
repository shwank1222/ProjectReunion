#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FOSMKEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<class IPropertyTypeIdentifier> PropertyPickerIdentifier;
	TSharedPtr<class IPropertyTypeIdentifier> FunctionPickerIdentifier;
};
