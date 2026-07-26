// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OSMKCharacterBase.h"
#include "Data/BulletData.h"
#include "Weapons/Bullets/BulletBase.h"
#include "PlayerCharacter.generated.h"

class UOSMKPostProcessManager;
class UOSMKSlowMotionSubsystem;
class ULevelSequence;
class ACutsceneActor;
struct FInputActionValue;
class UInputAction;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadedAmmoChanged);

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> FirstPersonPistol;

#pragma region Input

private:
	void MoveInput(const FInputActionValue& Value);
	void LookInput(const FInputActionValue& Value);
	
	void StartFiring();
	void OnHoldTriggered();
	void CancelFiring();
	void Fire();
	
	void StopSlowMotion();

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;
	
	UPROPERTY(EditDefaultsOnly)
	float AutoFireDuration = 0.5f;
	UPROPERTY(EditDefaultsOnly)
	float PostAutoFireDelay = 0.5f;
	
	FTimerHandle AutoFireTimerHandle;
	FTimerHandle RestoreTimerHandle;
	
	uint8 bIsFiring : 1 = false;
	uint8 bIsFired : 1 = false;
	
#pragma endregion

#pragma region Weapon

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AddAmmo(const FName RowName);
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void RestoreAmmo();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ResetAmmo();
	
	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE TArray<FBulletData> GetLoadedAmmo() const { return LoadedAmmo; }
	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE int32 NumOfLoadedAmmo() const { return LoadedAmmo.Num(); }

	UPROPERTY(BlueprintAssignable)
	FOnLoadedAmmoChanged OnLoadedAmmoChanged;
	
private:
	void FireProjectile(const TSubclassOf<ABulletBase> BulletClass);
	FVector GetWeaponTargetLocation() const;
	FTransform CalculateProjectileSpawnTransform(const FVector& TargetLocation) const;
	
	void PlayHeartPulseSound();
	void StopHeartPulseSound() const;

	FBulletData* GetBulletData(const FName RowName) const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TObjectPtr<USoundBase> HeartPulseSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MuzzleOffset = 10.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MaxAimDistance = 10000.0f;
	
	int32 MaxAmmoCount = 6;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TObjectPtr<UDataTable> BulletDataTable;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TArray<FBulletData> LoadedAmmo;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> HeartPulseSoundComponent;

#pragma endregion
	
private:
	UPROPERTY()
	TObjectPtr<UOSMKSlowMotionSubsystem> SlowMotionSubsystem;
};
