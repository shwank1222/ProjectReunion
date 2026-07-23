// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OSMKCharacterBase.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class UInputAction;
class UCameraComponent;

UCLASS()
class OSMK_API APlayerCharacter : public AOSMKCharacterBase
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(Blueprintpure)
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	UFUNCTION(Blueprintpure)
	FORCEINLINE UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> FirstPersonPistol;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> ThirdPersonPistol;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> FireAnimMontage;
	
#pragma region Input
	
private:
	void MoveInput(const FInputActionValue& Value);
	void LookInput(const FInputActionValue& Value);
	void Fire();
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;
	
#pragma endregion
};
