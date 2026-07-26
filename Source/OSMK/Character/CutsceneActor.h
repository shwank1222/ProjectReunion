// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CutsceneActor.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class OSMK_API ACutsceneActor : public AActor
{
	GENERATED_BODY()

public:
	ACutsceneActor();
	
	void ShowActor(const bool bClear);
	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> BoxMeshComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMesh> PlayerSkeletalMesh;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMesh> EnemySkeletalMesh;
};
