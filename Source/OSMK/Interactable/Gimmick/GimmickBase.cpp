#include "Interactable/Gimmick/GimmickBase.h"
#include "Character/OSMKCharacterBase.h"

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

void AGimmickBase::HandleCharacterHit(AOSMKCharacterBase* Character) const
{
	if (!Character)
	{
		return;
	}

	GIMMICK_LOG(
		Log,
		TEXT("Character hit: %s"),
		*GetNameSafe(Character));
	
	Character->ApplyDamage();
}
