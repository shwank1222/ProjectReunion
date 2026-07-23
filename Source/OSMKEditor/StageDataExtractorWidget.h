#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Classes/EditorUtilityWidget.h"
#include "StageDataExtractorWidget.generated.h"

class UButton;
class UDataTable;

UCLASS()
class UStageDataExtractorWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnExtractButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* ExtractButton = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	class UDetailsView* LevelDetailsView = nullptr;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	UDataTable* StaticMeshDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	UDataTable* EnemyDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	UDataTable* GimmickDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Data Extractor")
	TArray<TSoftObjectPtr<UWorld>> TargetLevels;
};