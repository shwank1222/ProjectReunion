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
class UCameraShakeBase;
class APlayerCameraManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadedAmmoChanged);

UCLASS()
class OSMK_API APlayerCharacter : public AOSMKCharacterBase
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure)
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	FVector GetHeadWorldLocation() const;

	void PrepareForVictoryShot();

	void LockForReplay();

protected:
	virtual void Die() override;
	virtual void PrepareForReplay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> FirstPersonPistol = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FName HeadSocketName = FName("head");

	static constexpr float HeadFallbackHeight = 65.0f;

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
	TObjectPtr<UInputAction> MoveAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float AutoFireDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float PostAutoFireDelay = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion", meta = (ClampMin = "0.125", ClampMax = "1.0"))
	float SlowFireSoundPitch = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion", meta = (ClampMin = "0.0"))
	float SlowFireSoundFadeTime = 0.2f;

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
	TObjectPtr<USoundBase> HeartPulseSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MuzzleOffset = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MaxAimDistance = 10000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = "1"))
	int32 MaxAmmoCount = 6;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TObjectPtr<UDataTable> BulletDataTable = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TArray<FBulletData> LoadedAmmo;

	UPROPERTY()
	TObjectPtr<UAudioComponent> HeartPulseSoundComponent = nullptr;

#pragma endregion

#pragma region CameraShake

private:
	void UpdateWalkCameraShake();
	void StopWalkCameraShake();
	void StartFireRecoil();
	void UpdateFireRecoil();
	void StopFireRecoil();

	static constexpr float WalkShakeCheckInterval = 0.1f;

	FTimerHandle WalkShakeTimerHandle;
	FTimerHandle RecoilTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> WalkCameraShake = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "CameraShake", meta = (ClampMin = "0.0"))
	float WalkShakeMinSpeed = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "CameraShake", meta = (ClampMin = "0.0"))
	float WalkShakeScale = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "CameraShake|Recoil")
	float FireRecoilPitch = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "CameraShake|Recoil", meta = (ClampMin = "0.0"))
	float FireRecoilYaw = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "CameraShake|Recoil", meta = (ClampMin = "0.0"))
	float FireRecoilKickDuration = 0.05f;

	UPROPERTY(EditDefaultsOnly, Category = "CameraShake|Recoil", meta = (ClampMin = "0.0"))
	float FireRecoilRecoverDuration = 0.25f;

	FRotator RecoilPeak = FRotator::ZeroRotator;
	float RecoilElapsed = 0.0f;
	bool bRecoilActive = false;

	UPROPERTY(Transient)
	TObjectPtr<UCameraShakeBase> ActiveWalkShake = nullptr;

	TWeakObjectPtr<APlayerCameraManager> WalkShakeCameraManager;

#pragma endregion

private:
	UPROPERTY()
	TObjectPtr<UOSMKSlowMotionSubsystem> SlowMotionSubsystem = nullptr;
};
