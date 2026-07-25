// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OSMKCutsceneManager.generated.h"

class ULevelSequencePlayer;
class ACutsceneActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCutsceneFinished);

UCLASS()
class OSMK_API UOSMKCutsceneManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	static UOSMKCutsceneManager* Get(const UObject* WorldContextObject);
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	void PlayCutscene(const bool bClear) const;
	
	UPROPERTY(BlueprintAssignable)
	FOnCutsceneFinished OnCutsceneFinished;
	
private:
	UFUNCTION()
	void HandleCutsceneFinished();
	
	UPROPERTY()
	TObjectPtr<ACutsceneActor> CutsceneActor;
	
	UPROPERTY()
	TObjectPtr<ULevelSequencePlayer> LevelSequencePlayer;
};
