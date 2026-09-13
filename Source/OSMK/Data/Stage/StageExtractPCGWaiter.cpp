#include "Data/Stage/StageExtractPCGWaiter.h"
#include "PCGComponent.h"

void UStageExtractPCGWaiter::HandlePCGGenerated(UPCGComponent* InComponent)
{
	if (OnComplete)
	{
		OnComplete();
		OnComplete = nullptr;
	}
	RemoveFromRoot();
	MarkAsGarbage();
}
