#include "OSMKBGMSubsystem.h"
#include "Components/AudioComponent.h"
#include "OSMKGameInstance.h"
#include "Data/SoundData.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundClass.h"

void UOSMKBGMSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	WorldChangedHandle = FWorldDelegates::OnPostWorldInitialization.AddUObject(
		this, &UOSMKBGMSubsystem::OnWorldChanged);
}

void UOSMKBGMSubsystem::Deinitialize()
{
	FWorldDelegates::OnPostWorldInitialization.Remove(WorldChangedHandle);

	if (BGMAudioComponent)
	{
		BGMAudioComponent->Stop();
		BGMAudioComponent = nullptr;
	}

	Super::Deinitialize();
}

void UOSMKBGMSubsystem::PlayBGM(USoundBase* NewBGM, float FadeInDuration)
{
	if (!NewBGM)
	{
		return;
	}

	PendingBGM            = NewBGM;
	PendingFadeInDuration = FadeInDuration;

	if (BGMAudioComponent && BGMAudioComponent->IsPlaying())
	{
		FadeOut(1.0f, [this]()
		{
			FadeIn(PendingFadeInDuration);
		});
	}
	else
	{
		FadeIn(FadeInDuration);
	}
}

void UOSMKBGMSubsystem::StopBGM(float FadeOutDuration)
{
	if (!BGMAudioComponent || !BGMAudioComponent->IsPlaying())
	{
		return;
	}

	FadeOut(FadeOutDuration);
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

	TSoftObjectPtr<UWorld> WorldRef(World);

	TSoftObjectPtr<USoundBase>* FoundBGM = GI->SoundData->LevelBGMMap.Find(WorldRef);
	USoundBase* TargetBGM = FoundBGM ? FoundBGM->LoadSynchronous() : GI->SoundData->DefaultBGM.LoadSynchronous();

	if (TargetBGM)
	{
		World->GetTimerManager().SetTimerForNextTick([this, TargetBGM]()
		{
			PlayBGM(TargetBGM);
		});
	}
	else
	{
		World->GetTimerManager().SetTimerForNextTick([this]()
		{
			StopBGM();
		});
	}
}

void UOSMKBGMSubsystem::FadeIn(float Duration)
{
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World || !PendingBGM)
	{
		return;
	}

	if (!BGMAudioComponent)
	{
		BGMAudioComponent = UGameplayStatics::CreateSound2D(World, PendingBGM, 1.0f, 1.0f, 0.0f, nullptr, true, false);
	}
	else
	{
		BGMAudioComponent->SetSound(PendingBGM);
	}

	if (BGMAudioComponent)
	{
		BGMAudioComponent->FadeIn(Duration, 1.0f);
	}
}

void UOSMKBGMSubsystem::FadeOut(float Duration, TFunction<void()> OnComplete)
{
	if (!BGMAudioComponent)
	{
		return;
	}

	BGMAudioComponent->FadeOut(Duration, 0.0f);

	if (OnComplete)
	{
		UWorld* World = GetGameInstance()->GetWorld();
		if (World)
		{
			World->GetTimerManager().SetTimer(FadeTimerHandle, [OnComplete]()
			{
				OnComplete();
			}, Duration, false);
		}
	}
}
