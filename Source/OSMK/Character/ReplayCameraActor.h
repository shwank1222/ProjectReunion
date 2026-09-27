// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReplayCameraActor.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UNiagaraSystem;
class AOSMKCharacterBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReplayFinished);

UCLASS()
class OSMK_API AReplayCameraActor : public AActor
{
	GENERATED_BODY()

public:
	AReplayCameraActor();

	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void StartReplay(AOSMKCharacterBase* Target);

	void BeginHoldShot(AOSMKCharacterBase* HoldTarget, bool bUseFrontAngle);

	UFUNCTION(BlueprintCallable, Category = "Replay")
	void SetReplayTarget(AOSMKCharacterBase* Target) { TargetCharacter = Target; }

	UFUNCTION(BlueprintCallable, Category = "Replay")
	void HideActorsBetweenCameraAndTarget();

	UFUNCTION(BlueprintCallable, Category = "Replay")
	void RestoreHiddenActors();

private:
	void SetupAngle(int32 AngleIndex);
	void ApplyCameraPlacement(const FVector& PivotPos, const FVector& CameraPos);
	FVector CalculateCameraPosition(int32 AngleIndex) const;
	FVector GetHoldPivotLocation() const;

	TArray<FHitResult> GatherLineTraceActors(const FCollisionQueryParams& Params) const;
	TArray<AActor*> GatherSphereOverlapActors(const FCollisionQueryParams& Params) const;
	bool ShouldHideActor(const AActor* Actor) const;

	UFUNCTION()
	void OnRecordingComplete();

	void OnAngleComplete();
	void FinishReplay();

public:
	UPROPERTY(BlueprintAssignable)
	FOnReplayFinished OnReplayFinished;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Replay")
	TObjectPtr<UNiagaraSystem> HitEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Replay")
	float CameraDistance = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Replay")
	float CameraHeight = 80.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Replay")
	float RecordDuration = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Replay")
	float ReplayTimeDilation = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Replay")
	float CameraProbeRadius = 50.0f;

private:
	static constexpr int32 ReplayAngleCount = 3;

	static constexpr float SlowMotionHoldDuration = 10000.0f;

	UPROPERTY()
	TObjectPtr<AOSMKCharacterBase> TargetCharacter = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> HiddenActors;

	UPROPERTY()
	TArray<TObjectPtr<UPrimitiveComponent>> HiddenComponents;

	int32 CurrentAngleIndex = 0;

	double PlaybackStartRealTime = 0.0;

	uint8 bIsPlayingBack : 1 = false;

	uint8 bIsHoldingShot : 1 = false;

	FTimerHandle RecordingTimerHandle;
};
