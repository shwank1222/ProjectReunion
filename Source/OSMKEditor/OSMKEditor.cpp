#include "OSMKEditor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Customization/StageLevelConfigCustomization.h"
#include "Customization/StageMemberPickerCustomization.h"
#include "Data/StageData.h"

IMPLEMENT_MODULE(FOSMKEditorModule, OSMKEditor);

void FOSMKEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(
		FStageLevelConfig::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStageLevelConfigCustomization::MakeInstance)
	);

	PropertyPickerIdentifier = MakeShared<FStageMemberPickerIdentifier>(TEXT("StagePropertyPicker"));
	FunctionPickerIdentifier = MakeShared<FStageMemberPickerIdentifier>(TEXT("StageFunctionPicker"));

	PropertyModule.RegisterCustomPropertyTypeLayout(
		NAME_NameProperty,
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStageMemberPickerCustomization::MakeInstance, EStageMemberPickerMode::Property),
		PropertyPickerIdentifier
	);
	PropertyModule.RegisterCustomPropertyTypeLayout(
		NAME_NameProperty,
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStageMemberPickerCustomization::MakeInstance, EStageMemberPickerMode::Function),
		FunctionPickerIdentifier
	);
}

void FOSMKEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(FStageLevelConfig::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(NAME_NameProperty, PropertyPickerIdentifier);
		PropertyModule.UnregisterCustomPropertyTypeLayout(NAME_NameProperty, FunctionPickerIdentifier);
	}
}
