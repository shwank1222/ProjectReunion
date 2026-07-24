#include "StageLevelConfigCustomization.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "DetailLayoutBuilder.h"
#include "Engine/DataTable.h"
#include "Data/StageData.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"

TSharedRef<IPropertyTypeCustomization> FStageLevelConfigCustomization::MakeInstance()
{
	return MakeShareable(new FStageLevelConfigCustomization());
}

void FStageLevelConfigCustomization::CustomizeHeader(
	TSharedRef<IPropertyHandle> StructPropertyHandle,
	FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> StageRowNameHandle = StructPropertyHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FStageLevelConfig, StageRowName));

	UnlockedBulletRowNamesHandle = StructPropertyHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FStageLevelConfig, UnlockedBulletRowNames));

	FName RowName = NAME_None;
	if (StageRowNameHandle.IsValid())
	{
		StageRowNameHandle->GetValue(RowName);
	}

	HeaderRow
		.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromName(RowName))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];
}

void FStageLevelConfigCustomization::CustomizeChildren(
	TSharedRef<IPropertyHandle> StructPropertyHandle,
	IDetailChildrenBuilder& ChildBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	UDataTable* BulletDT = GetBulletDataTable(StructPropertyHandle);
	if (!BulletDT)
	{
		return;
	}

	for (const FName& RowName : BulletDT->GetRowNames())
	{
		ChildBuilder.AddCustomRow(FText::FromName(RowName))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromName(RowName))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		[
			SNew(SCheckBox)
			.IsChecked_Raw(this, &FStageLevelConfigCustomization::GetCheckBoxState, RowName)
			.OnCheckStateChanged_Raw(this, &FStageLevelConfigCustomization::OnCheckBoxChanged, RowName)
		];
	}
}

UDataTable* FStageLevelConfigCustomization::GetBulletDataTable(TSharedRef<IPropertyHandle> StructPropertyHandle) const
{
	TArray<UObject*> OuterObjects;
	StructPropertyHandle->GetOuterObjects(OuterObjects);
	for (UObject* Obj : OuterObjects)
	{
		if (UStageData* StageData = Cast<UStageData>(Obj))
		{
			return StageData->BulletDataTable;
		}
	}
	return nullptr;
}

TArray<FName> FStageLevelConfigCustomization::GetCurrentUnlockedNames() const
{
	TArray<FName> Result;
	if (!UnlockedBulletRowNamesHandle.IsValid())
	{
		return Result;
	}

	TSharedPtr<IPropertyHandleArray> ArrayHandle = UnlockedBulletRowNamesHandle->AsArray();
	if (!ArrayHandle.IsValid())
	{
		return Result;
	}

	uint32 NumElements = 0;
	ArrayHandle->GetNumElements(NumElements);
	for (uint32 i = 0; i < NumElements; i++)
	{
		FName Name;
		ArrayHandle->GetElement(i)->GetValue(Name);
		Result.Add(Name);
	}
	return Result;
}

ECheckBoxState FStageLevelConfigCustomization::GetCheckBoxState(FName RowName) const
{
	return GetCurrentUnlockedNames().Contains(RowName) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void FStageLevelConfigCustomization::OnCheckBoxChanged(ECheckBoxState NewState, FName RowName)
{
	if (!UnlockedBulletRowNamesHandle.IsValid())
	{
		return;
	}

	TArray<FName> Current = GetCurrentUnlockedNames();
	if (NewState == ECheckBoxState::Checked)
	{
		Current.AddUnique(RowName);
	}
	else
	{
		Current.Remove(RowName);
	}

	TSharedPtr<IPropertyHandleArray> ArrayHandle = UnlockedBulletRowNamesHandle->AsArray();
	if (!ArrayHandle.IsValid())
	{
		return;
	}

	UnlockedBulletRowNamesHandle->NotifyPreChange();

	ArrayHandle->EmptyArray();
	for (const FName& Name : Current)
	{
		ArrayHandle->AddItem();
		uint32 NumElements = 0;
		ArrayHandle->GetNumElements(NumElements);
		ArrayHandle->GetElement(NumElements - 1)->SetValue(Name);
	}

	UnlockedBulletRowNamesHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
}
