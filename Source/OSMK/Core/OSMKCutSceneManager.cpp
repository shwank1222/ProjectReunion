// Fill out your copyright notice in the Description page of Project Settings.


#include "OSMKCutSceneManager.h"

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Character/CutSceneActor.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogCutSceneManager, Log, All);

UOSMKCutSceneManager* UOSMKCutSceneManager::Get(const UObject* WorldContextObject)
{
	return WorldContextObject->GetWorld()->GetSubsystem<UOSMKCutSceneManager>();
}

bool UOSMKCutSceneManager::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	if (!IsValid(Outer))
	{
		return false;
	}

	UWorld* World = Outer->GetWorld();
	if (!IsValid(World))
	{
		return false;
	}

	UE_LOG(LogCutSceneManager, Warning, TEXT("World Name: %s"), *UGameplayStatics::GetCurrentLevelName(World, true));

	return true;
}

void UOSMKCutSceneManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (const ALevelSequenceActor* LevelSequenceActor = Cast<ALevelSequenceActor>(
		UGameplayStatics::GetActorOfClass(&InWorld, ALevelSequenceActor::StaticClass())))
	{
		LevelSequencePlayer = LevelSequenceActor->GetSequencePlayer();
		LevelSequencePlayer->OnFinished.AddUniqueDynamic(this, &ThisClass::HandleCutSceneFinished);
	}

	CutSceneActor = Cast<ACutSceneActor>(UGameplayStatics::GetActorOfClass(&InWorld, ACutSceneActor::StaticClass()));
}

void UOSMKCutSceneManager::PlayCutScene(AActor* SourceActor, const bool bClear) const
{
	if (!IsValid(SourceActor)
		|| !IsValid(CutSceneActor)
		|| !IsValid(LevelSequencePlayer))
	{
		UE_LOG(LogCutSceneManager, Error, TEXT("Play CutScene Failed"));
		return;
	}

	SourceActor->SetActorHiddenInGame(true);

	CutSceneActor->ShowActor(SourceActor->GetTransform(), bClear);

	LevelSequencePlayer->Play();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UOSMKCutSceneManager::HandleCutSceneFinished()
{
	CutSceneActor->HideActor();
	
	OnCutSceneFinished.Broadcast();
}
