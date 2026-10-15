#include "OSMKBGMSubsystem.h"
#include "Components/AudioComponent.h"
#include "OSMKGameInstance.h"
#include "Data/SoundData.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

namespace
{
	constexpr float MutedLayerVolume = 0.0001f;
}

void UOSMKBGMSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	WorldChangedHandle = FWorldDelegates::OnPostWorldInitialization.AddUObject(
		this, &UOSMKBGMSubsystem::OnWorldChanged);
}

void UOSMKBGMSubsystem::Deinitialize()
{
	FWorldDelegates::OnPostWorldInitialization.Remove(WorldChangedHandle);

	if (CalmLayerComponent)
	{
		CalmLayerComponent->Stop();
		CalmLayerComponent = nullptr;
	}

	if (CombatLayerComponent)
	{
		CombatLayerComponent->Stop();
		CombatLayerComponent = nullptr;
	}

	Super::Deinitialize();
}

void UOSMKBGMSubsystem::OnWorldChanged(UWorld* World, const UWorld::InitializationValues IVS)
{
	if (!World || World->IsPreviewWorld())
	{
		return;
	}

	UOSMKGameInstance* GI = Cast<UOSMKGameInstance>(GetGameInstance());
	if (!GI || !GI->SoundData)
	{
		return;
	}

	USoundBase* CalmSound = GI->SoundData->CalmLayerBGM.LoadSynchronous();
	USoundBase* CombatSound = GI->SoundData->CombatLayerBGM.LoadSynchronous();

	World->GetTimerManager().SetTimerForNextTick([this, GI, CalmSound, CombatSound]()
	{
		GI->ApplySoundSettings();

		if (IsLayerPlaying())
		{
			SetCombatLayer(false);
			return;
		}

		StartLayerBGM(CalmSound, CombatSound, 1.0f);
	});
}

bool UOSMKBGMSubsystem::IsLayerPlaying() const
{
	return CalmLayerComponent && CombatLayerComponent && CalmLayerComponent->IsPlaying() && CombatLayerComponent->IsPlaying();
}

void UOSMKBGMSubsystem::StartLayerBGM(USoundBase* CalmSound, USoundBase* CombatSound, float FadeInDuration)
{
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World || !CalmSound || !CombatSound)
	{
		return;
	}

	if (!CalmLayerComponent)
	{
		CalmLayerComponent = UGameplayStatics::CreateSound2D(World, CalmSound, 1.0f, 1.0f, 0.0f, nullptr, true, false);
	}
	else
	{
		CalmLayerComponent->SetSound(CalmSound);
	}

	if (!CombatLayerComponent)
	{
		CombatLayerComponent = UGameplayStatics::CreateSound2D(World, CombatSound, 1.0f, 1.0f, 0.0f, nullptr, true, false);
	}
	else
	{
		CombatLayerComponent->SetSound(CombatSound);
	}

	if (!CalmLayerComponent || !CombatLayerComponent)
	{
		return;
	}

	CalmLayerComponent->bIsUISound = false;
	CombatLayerComponent->bIsUISound = false;

	bCombatLayer = false;
	CalmLayerComponent->FadeIn(FadeInDuration, 1.0f);
	CombatLayerComponent->FadeIn(FadeInDuration, MutedLayerVolume);
}

void UOSMKBGMSubsystem::SetCombatLayer(bool bCombat)
{
	if (!IsLayerPlaying() || bCombatLayer == bCombat)
	{
		return;
	}

	bCombatLayer = bCombat;

	const float Duration = GetLayerCrossfadeDuration();
	CalmLayerComponent->AdjustVolume(Duration, bCombat ? MutedLayerVolume : 1.0f);
	CombatLayerComponent->AdjustVolume(Duration, bCombat ? 1.0f : MutedLayerVolume);
}

float UOSMKBGMSubsystem::GetLayerCrossfadeDuration() const
{
	const UOSMKGameInstance* GI = Cast<UOSMKGameInstance>(GetGameInstance());
	return GI && GI->SoundData ? GI->SoundData->LayerCrossfadeDuration : 2.0f;
}
