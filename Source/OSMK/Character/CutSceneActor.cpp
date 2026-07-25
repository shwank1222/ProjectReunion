// Fill out your copyright notice in the Description page of Project Settings.


#include "CutSceneActor.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACutSceneActor::ACutSceneActor()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	SkeletalMeshComponent->SetupAttachment(SceneComponent);
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(SceneComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

void ACutSceneActor::ShowActor(const FTransform& Transform, const bool bClear)
{
	SetActorTransform(Transform);
	
	SkeletalMeshComponent->SetSkeletalMesh(bClear ? EnemySkeletalMesh : PlayerSkeletalMesh);
	
	SetActorHiddenInGame(false);
}

void ACutSceneActor::HideActor()
{
	SetActorHiddenInGame(true);
	
	SetActorTransform(FTransform::Identity);
}
