#include "StageMemberPickerCustomization.h"
#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "Data/Stage/StageExtractRule.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/SBoxPanel.h"

TSharedRef<IPropertyTypeCustomization> FStageMemberPickerCustomization::MakeInstance(EStageMemberPickerMode InMode)
{
	return MakeShared<FStageMemberPickerCustomization>(InMode);
}

void FStageMemberPickerCustomization::CustomizeHeader(
	TSharedRef<IPropertyHandle> PropertyHandle,
	FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	Handle = PropertyHandle;

	HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(250.f)
		[
			SAssignNew(ComboButton, SComboButton)
			.OnGetMenuContent(this, &FStageMemberPickerCustomization::BuildMenu)
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text(this, &FStageMemberPickerCustomization::GetValueText)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
		];
}

TSharedRef<SWidget> FStageMemberPickerCustomization::BuildMenu()
{
	AllOptions.Reset();
	AllOptions.Add(MakeShared<FString>(FName(NAME_None).ToString()));
	for (const FString& Option : GatherOptions())
	{
		AllOptions.Add(MakeShared<FString>(Option));
	}
	FilteredOptions = AllOptions;

	TSharedRef<SSearchBox> SearchBox = SNew(SSearchBox)
		.OnTextChanged(this, &FStageMemberPickerCustomization::OnFilterChanged);

	ComboButton->SetMenuContentWidgetToFocus(SearchBox);

	return SNew(SBox)
		.MinDesiredWidth(300.f)
		.MaxDesiredHeight(400.f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(4.f)
			[
				SearchBox
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.f)
			[
				SAssignNew(ListView, SListView<TSharedPtr<FString>>)
				.ListItemsSource(&FilteredOptions)
				.SelectionMode(ESelectionMode::Single)
				.OnGenerateRow(this, &FStageMemberPickerCustomization::GenerateRow)
				.OnSelectionChanged(this, &FStageMemberPickerCustomization::OnSelectionChanged)
			]
		];
}

TArray<FString> FStageMemberPickerCustomization::GatherOptions() const
{
	TArray<UObject*> OuterObjects;
	Handle->GetOuterObjects(OuterObjects);

	for (const UObject* Outer : OuterObjects)
	{
		if (const UStageExtractRule* Rule = Cast<UStageExtractRule>(Outer))
		{
			return Mode == EStageMemberPickerMode::Property ? Rule->GetPropertyOptions() : Rule->GetFunctionOptions();
		}
	}

	return TArray<FString>();
}

FText FStageMemberPickerCustomization::GetValueText() const
{
	FName Value = NAME_None;
	if (Handle->GetValue(Value) == FPropertyAccess::MultipleValues)
	{
		return NSLOCTEXT("StageMemberPicker", "MultipleValues", "Multiple Values");
	}
	return FText::FromName(Value);
}

void FStageMemberPickerCustomization::OnFilterChanged(const FText& FilterText)
{
	const FString Filter = FilterText.ToString();

	FilteredOptions.Reset();
	for (const TSharedPtr<FString>& Option : AllOptions)
	{
		if (Filter.IsEmpty() || Option->Contains(Filter))
		{
			FilteredOptions.Add(Option);
		}
	}

	if (ListView.IsValid())
	{
		ListView->RequestListRefresh();
	}
}

TSharedRef<ITableRow> FStageMemberPickerCustomization::GenerateRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		[
			SNew(STextBlock)
			.Text(FText::FromString(*Item))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];
}

void FStageMemberPickerCustomization::OnSelectionChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo)
{
	if (!Item.IsValid() || SelectInfo == ESelectInfo::Direct)
	{
		return;
	}

	Handle->SetValue(FName(**Item));
	ComboButton->SetIsOpen(false);
}
