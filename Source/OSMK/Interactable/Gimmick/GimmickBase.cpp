#include "Interactable/Gimmick/GimmickBase.h"
#include "Character/OSMKCharacterBase.h"
#include "Core/OSMKGameState.h"

DEFINE_LOG_CATEGORY(LogGimmick);

AGimmickBase::AGimmickBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
}

void AGimmickBase::Trigger()
{
	if (bTriggered)
	{
		return;
	}

	bTriggered = true;

	if (AOSMKGameState* GS = GetWorld()->GetGameState<AOSMKGameState>())
	{
		GS->NotifyGimmickTriggered();
	}

	OnTriggered();
}

bool AGimmickBase::CanTrigger() const
{
	return !bTriggered;
}


void AGimmickBase::BeginPlay()
{
	Super::BeginPlay();
	
	SetCustomStencil();
}

void AGimmickBase::SetCustomStencil() const
{
	TArray<UMeshComponent*> MeshComponents;
	GetComponents(UMeshComponent::StaticClass(), MeshComponents);
	
	for (UMeshComponent* MeshComponent : MeshComponents)
	{
		MeshComponent->SetRenderCustomDepth(true);
		MeshComponent->SetCustomDepthStencilValue(1);
	}
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
