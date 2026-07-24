#pragma once

#include "Interactable/Gimmick/GimmickBase.h"
#include "HangingTrap.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UCableComponent;
class USoundBase;
class UNiagaraSystem;

UCLASS()
class OSMK_API AHangingTrap : public AGimmickBase
{
	GENERATED_BODY()
	
public:
	AHangingTrap();
	
protected:
	
	UFUNCTION()
	void OnRebarHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);
	
	virtual void BeginPlay() override;
	
	virtual void OnTriggered() override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;
	
	// Trigger
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> TriggerCollision;
	
	// Hanging cables
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCableComponent> Cable;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> CableEndPoint;
	
	// Falling meshes
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UStaticMeshComponent>> Rebars;
	
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TObjectPtr<UNiagaraSystem> DustEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TObjectPtr<USoundBase> CollisionSound;

	
private:
	bool bHasHitGround=false;;
};
