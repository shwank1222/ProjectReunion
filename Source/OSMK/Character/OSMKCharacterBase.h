// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OSMKCharacterBase.generated.h"

UCLASS()
class OSMK_API AOSMKCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AOSMKCharacterBase();
	
	void EnableRagdoll();
};
