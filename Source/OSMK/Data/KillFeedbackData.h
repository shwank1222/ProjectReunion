#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "KillFeedbackData.generated.h"

USTRUCT(BlueprintType)
struct FKillFeedbackData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Feedback")
	FText FeedbackText = FText::GetEmpty();
};