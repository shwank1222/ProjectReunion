// Fill out your copyright notice in the Description page of Project Settings.


#include "OSMKCutsceneManager.h"

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Character/CutSceneActor.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogCutsceneManager, Log, All);

UOSMKCutsceneManager* UOSMKCutsceneManager::Get(const UObject* WorldContextObject)
{
	return WorldContextObject->GetWorld()->GetSubsystem<UOSMKCutsceneManager>();
}

bool UOSMKCutsceneManager::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	if (!IsValid(World))
	{
		return false;
	}

	if (World->GetName().Contains("Title"))
	{
		return false;
	}

	return true;
}

void UOSMKCutsceneManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (const ALevelSequenceActor* LevelSequenceActor = Cast<ALevelSequenceActor>(
		UGameplayStatics::GetActorOfClass(&InWorld, ALevelSequenceActor::StaticClass())))
	{
		LevelSequencePlayer = LevelSequenceActor->GetSequencePlayer();
		LevelSequencePlayer->OnFinished.AddUniqueDynamic(this, &ThisClass::HandleCutsceneFinished);
	}

	CutsceneActor = Cast<ACutsceneActor>(UGameplayStatics::GetActorOfClass(&InWorld, ACutsceneActor::StaticClass()));
}

void UOSMKCutsceneManager::PlayCutscene(const bool bClear) const
{
	if (!IsValid(CutsceneActor) || !IsValid(LevelSequencePlayer))
	{
		UE_LOG(LogCutsceneManager, Error, TEXT("Play Cutscene Failed"));
		return;
	}

	CutsceneActor->ShowActor(bClear);

	LevelSequencePlayer->Play();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UOSMKCutsceneManager::HandleCutsceneFinished()
{
	CutsceneActor->SetActorHiddenInGame(true);

	OnCutsceneFinished.Broadcast();
}
