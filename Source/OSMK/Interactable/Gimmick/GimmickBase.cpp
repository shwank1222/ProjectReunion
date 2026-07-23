#include "Interactable/Gimmick/GimmickBase.h"

DEFINE_LOG_CATEGORY(LogGimmick);

AGimmickBase::AGimmickBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGimmickBase::Trigger()
{
	if (bTriggered)
	{
		return;
	}

	bTriggered = true;

	OnTriggered();
}

bool AGimmickBase::CanTrigger() const
{
	return !bTriggered;
}


void AGimmickBase::BeginPlay()
{
	Super::BeginPlay();
}
