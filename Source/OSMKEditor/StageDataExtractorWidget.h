#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Classes/EditorUtilityWidget.h"
#include "StageDataExtractorWidget.generated.h"

class UButton;
class UStageData;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Data Extractor")
	UStageData* TargetStageData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Data Extractor")
	TArray<TSoftObjectPtr<UWorld>> TargetLevels;
};
