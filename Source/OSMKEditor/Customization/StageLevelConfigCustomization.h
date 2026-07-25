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
	class UDataTable* GetBulletDataTable(TSharedRef<IPropertyHandle> InStructHandle) const;
	TOptional<int32> GetBulletCount(FName RowName) const;
	void SetBulletCount(int32 NewValue, ETextCommit::Type, FName RowName);
	
private:
	TSharedPtr<IPropertyHandle> StructHandle = nullptr;	
};
