// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OSMKCutSceneManager.generated.h"

class ULevelSequencePlayer;
class ACutSceneActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCutSceneFinished);

UCLASS()
class OSMK_API UOSMKCutSceneManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	static UOSMKCutSceneManager* Get(const UObject* WorldContextObject);
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	void PlayCutScene(AActor* SourceActor, const bool bClear) const;
	
	UPROPERTY(BlueprintAssignable)
	FOnCutSceneFinished OnCutSceneFinished;
	
private:
	UFUNCTION()
	void HandleCutSceneFinished();
	
	UPROPERTY()
	TObjectPtr<ACutSceneActor> CutSceneActor;
	
	UPROPERTY()
	TObjectPtr<ULevelSequencePlayer> LevelSequencePlayer;
};
