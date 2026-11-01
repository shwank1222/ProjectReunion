#pragma once

#include "Interactable/Gimmick/GimmickBase.h"
#include "PressurePipe.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UArrowComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;
class UAudioComponent;

UCLASS()
class OSMK_API APressurePipe : public AGimmickBase
{
	GENERATED_BODY()
	
public:
	APressurePipe();
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
#pragma region Components and Effects
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> PipeMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> BurstPoint;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BurstCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> BurstArrow;
	
	UPROPERTY(EditDefaultsOnly, Category="Burst|Effects")
	TObjectPtr<UNiagaraSystem> BurstEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="Burst|Effects")
	TObjectPtr<USoundBase> BurstSound;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> BurstAudioComponent;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> EffectComponent;
	
#pragma endregion
	
#pragma region Trigger

	UFUNCTION()
	void OnBurstOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	virtual void OnTriggered() override;
	
	UFUNCTION(BlueprintCallable, Category = "Gimmick|PressurePipe")
	void ActivateRelease();
	
	UFUNCTION(BlueprintCallable, Category = "Gimmick|PressurePipe")
	void DeactivateRelease();
	
private:
	
	void Burst();

#pragma endregion
	
#pragma region Parameters
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Burst|Parameters")
	float BurstDelay = 1.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Burst|Parameters")
	float BurstDuration = 30.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Burst|Parameters")
	float EffectScale = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category="Burst|Parameters")
	float VolumeMultiplier = 1.f;

private:
	FTimerHandle BurstDelayTimerHandle;
	FTimerHandle BurstDurationTimerHandle;
	
#pragma endregion
};
