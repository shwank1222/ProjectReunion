#include "OSMKEditor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Customization/StageLevelConfigCustomization.h"
#include "Data/StageData.h"

IMPLEMENT_MODULE(FOSMKEditorModule, OSMKEditor);

void FOSMKEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(
		FStageLevelConfig::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStageLevelConfigCustomization::MakeInstance)
	);
}

void FOSMKEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(FStageLevelConfig::StaticStruct()->GetFName());
	}
}
