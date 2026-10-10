#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "PropertyEditorModule.h"

class SComboButton;
class ITableRow;
class STableViewBase;
template <typename ItemType> class SListView;

enum class EStageMemberPickerMode : uint8
{
	Property,
	Function
};

class FStageMemberPickerCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance(EStageMemberPickerMode InMode);

	explicit FStageMemberPickerCustomization(EStageMemberPickerMode InMode) : Mode(InMode) {}

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
		FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
		IDetailChildrenBuilder& ChildBuilder,
		IPropertyTypeCustomizationUtils& CustomizationUtils) override {}

private:
	TSharedRef<SWidget> BuildMenu();
	TArray<FString> GatherOptions() const;
	FText GetValueText() const;

	void OnFilterChanged(const FText& FilterText);
	TSharedRef<ITableRow> GenerateRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable);
	void OnSelectionChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo);

private:
	EStageMemberPickerMode Mode;
	TSharedPtr<IPropertyHandle> Handle;
	TSharedPtr<SComboButton> ComboButton;
	TSharedPtr<SListView<TSharedPtr<FString>>> ListView;

	TArray<TSharedPtr<FString>> AllOptions;
	TArray<TSharedPtr<FString>> FilteredOptions;
};

class FStageMemberPickerIdentifier : public IPropertyTypeIdentifier
{
public:
	explicit FStageMemberPickerIdentifier(FName InMetaDataKey) : MetaDataKey(InMetaDataKey) {}

	virtual bool IsPropertyTypeCustomized(const IPropertyHandle& PropertyHandle) const override
	{
		return PropertyHandle.HasMetaData(MetaDataKey);
	}

private:
	FName MetaDataKey;
};
