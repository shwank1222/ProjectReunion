// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CutSceneActor.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class OSMK_API ACutSceneActor : public AActor
{
	GENERATED_BODY()

public:
	ACutSceneActor();
	
	void ShowActor(const FTransform& Transform, const bool bClear);
	void HideActor();
	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	
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
