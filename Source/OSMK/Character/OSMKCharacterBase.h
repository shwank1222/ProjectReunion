// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OSMKCharacterBase.generated.h"

class UNiagaraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeath);
DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All);

UCLASS()
class OSMK_API AOSMKCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AOSMKCharacterBase();

	virtual void Tick(float DeltaTime) override;

	void ApplyDamage(const FVector& HitLocation = FVector::ZeroVector,
	                 const FVector& ImpulseDirection = FVector::ZeroVector);

	void StartRecording();
	void StopRecording();
	void PreparePlayback();

	void PlaybackAtTime(float ElapsedSeconds);

	FORCEINLINE float GetRecordedDuration() const
	{
		return RecordedFrameTimes.IsEmpty() ? 0.0f : RecordedFrameTimes.Last();
	}
	FORCEINLINE const FVector& GetLastHitLocation() const { return PendingHitLocation; }

	virtual void PrepareForReplay() {}
	virtual void FinalizeDeathAfterReplay() {}

	UPROPERTY(BlueprintAssignable)
	FOnCharacterDeath OnCharacterDeath;

protected:
	virtual void Die();

	void PlayFireMontage(const USkeletalMeshComponent* SkeletalMesh) const;
	void PlayFireSound() const;
	void PlayFireEffect() const;

private:
	void RecordFrame();
	void ApplyFrame(int32 FrameIndex);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> PistolMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> MuzzleEffect = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FName MuzzleSocketName = FName("Muzzle");

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> FireAnimMontage = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> FireSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float ImpulseStrength = 50000.0f;

	UPROPERTY(VisibleAnywhere)
	uint8 bIsDead : 1 = false;

	FVector PendingHitLocation = FVector::ZeroVector;
	FVector PendingImpulseDirection = FVector::ZeroVector;

private:
	uint8 bIsRecording : 1 = false;

	TArray<TArray<FTransform>> RecordedFrames;

	TArray<float> RecordedFrameTimes;

	double RecordingStartRealTime = 0.0;

	int32 PlaybackCursor = 0;
};
