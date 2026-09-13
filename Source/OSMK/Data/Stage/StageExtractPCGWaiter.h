#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StageExtractPCGWaiter.generated.h"

class UPCGComponent;

UCLASS()
class OSMK_API UStageExtractPCGWaiter : public UObject
{
	GENERATED_BODY()

public:
	TFunction<void()> OnComplete;

	void HandlePCGGenerated(UPCGComponent* InComponent);
};
