// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OSMKCharacterBase.h"
#include "Weapons/Bullets/BulletBase.h"
#include "PlayerCharacter.generated.h"

struct FBulletData;
struct FInputActionValue;
class UInputAction;
class UCameraComponent;

UCLASS()
class OSMK_API APlayerCharacter : public AOSMKCharacterBase
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(Blueprintpure)
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	UFUNCTION(Blueprintpure)
	FORCEINLINE UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	
protected:
	virtual void Die() override;
	virtual void EnableRagdoll() override;
	
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
	
	void PlayFireAnimation() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;
	
#pragma endregion
	
#pragma region Weapon
	
private:
	void FireProjectile(const TSubclassOf<ABulletBase> BulletClass);
	FVector GetWeaponTargetLocation() const;
	FTransform CalculateProjectileSpawnTransform(const FVector& TargetLocation) const;
	
	FBulletData* GetBulletData(const FName RowName) const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName MuzzleSocketName;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MuzzleOffset = 10.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MaxAimDistance = 10000.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TObjectPtr<UDataTable> BulletDataTable;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<FName> DefaultBulletNames;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TArray<FName> RemainingBulletNames;
	
#pragma endregion
};
