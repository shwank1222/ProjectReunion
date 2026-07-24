#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

class FStageLevelConfigCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
		FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
		IDetailChildrenBuilder& ChildBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:
	TSharedPtr<IPropertyHandle> UnlockedBulletRowNamesHandle;

	class UDataTable* GetBulletDataTable(TSharedRef<IPropertyHandle> StructPropertyHandle) const;
	TArray<FName> GetCurrentUnlockedNames() const;
	ECheckBoxState GetCheckBoxState(FName RowName) const;
	void OnCheckBoxChanged(ECheckBoxState NewState, FName RowName);
};
