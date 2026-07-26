// Fill out your copyright notice in the Description page of Project Settings.


#include "CutsceneActor.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACutsceneActor::ACutsceneActor()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	SkeletalMeshComponent->SetupAttachment(SceneComponent);
	
	BoxMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMeshComponent"));
	BoxMeshComponent->SetupAttachment(SceneComponent);
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(SceneComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

void ACutsceneActor::ShowActor(const bool bClear)
{
	SkeletalMeshComponent->SetSkeletalMesh(bClear ? EnemySkeletalMesh : PlayerSkeletalMesh);
	
	SetActorHiddenInGame(false);
}
