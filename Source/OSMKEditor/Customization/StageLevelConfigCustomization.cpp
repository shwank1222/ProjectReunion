#include "StageLevelConfigCustomization.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "DetailLayoutBuilder.h"
#include "Engine/DataTable.h"
#include "Data/StageData.h"
#include "Widgets/Input/SNumericEntryBox.h"
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
	StructHandle = StructPropertyHandle;

	TSharedPtr<IPropertyHandle> StageRowNameHandle = StructPropertyHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FStageLevelConfig, StageRowName));

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
				SNew(SNumericEntryBox<int32>)
				.Value(this, &FStageLevelConfigCustomization::GetBulletCount, RowName)
				.MinValue(0)
				.MaxValue(6)
				.MinSliderValue(0)
				.MaxSliderValue(6)
				.AllowSpin(true)
				.OnValueCommitted(this, &FStageLevelConfigCustomization::SetBulletCount, RowName)
			];
	}
}

UDataTable* FStageLevelConfigCustomization::GetBulletDataTable(TSharedRef<IPropertyHandle> InStructHandle) const
{
	TArray<UObject*> OuterObjects;
	InStructHandle->GetOuterObjects(OuterObjects);
	for (UObject* Obj : OuterObjects)
	{
		if (UStageData* StageData = Cast<UStageData>(Obj))
		{
			return StageData->BulletDataTable;
		}
	}
	return nullptr;
}

TOptional<int32> FStageLevelConfigCustomization::GetBulletCount(FName RowName) const
{
	if (!StructHandle.IsValid())
	{
		return 0;
	}

	void* StructData = nullptr;
	if (StructHandle->GetValueData(StructData) != FPropertyAccess::Success || !StructData)
	{
		return 0;
	}

	FStageLevelConfig* Config = static_cast<FStageLevelConfig*>(StructData);
	const int32* Found = Config->BulletCounts.Find(RowName);
	return Found ? *Found : 0;
}

void FStageLevelConfigCustomization::SetBulletCount(int32 NewValue, ETextCommit::Type, FName RowName)
{
	if (!StructHandle.IsValid())
	{
		return;
	}

	void* StructData = nullptr;
	if (StructHandle->GetValueData(StructData) != FPropertyAccess::Success || !StructData)
	{
		return;
	}

	StructHandle->NotifyPreChange();
	FStageLevelConfig* Config = static_cast<FStageLevelConfig*>(StructData);
	Config->BulletCounts.FindOrAdd(RowName) = FMath::Clamp(NewValue, 0, 6);
	StructHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
}
