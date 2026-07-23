#pragma once

#include "Interactable/Gimmick/GimmickBase.h"
#include "ExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class UNiagaraSystem;
class USoundBase;
class UNiagaraComponent;
class AOSMKCharacterBase;

UCLASS()
class OSMK_API AExplosiveBarrel : public AGimmickBase
{
	GENERATED_BODY()

public:
	AExplosiveBarrel();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> BarrelMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UGeometryCollectionComponent> GeometryCollection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Explosion")
	TObjectPtr<USceneComponent> IgnitionPoint;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> IgnitionComponent;
	
	virtual void OnTriggered() override;
	
	void Explode();

	void ScanExplosionRadius();
	
	void HandleCharacterHit(AOSMKCharacterBase* Character);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Explosion")
	float ExplosionImpulse = 1500.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Explosion")
	float ExplosionRadius = 500.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Explosion")
	float ExplosionDelay = 1.f;
	
	UPROPERTY(EditAnywhere, Category = "Explosion")
	float ExplosionStrain = 10000000.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Explosion")
	bool bChainReaction = true;
	
	UPROPERTY(EditDefaultsOnly, Category="Explosion")
	TObjectPtr<UNiagaraSystem> IgnitionEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="Explosion")
	TObjectPtr<UNiagaraSystem> ExplosionEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="Explosion")
	TObjectPtr<USoundBase> ExplosionSound;

	UPROPERTY(EditAnywhere, Category="Explosion")
	bool bShowExplosionDebug = true;
private:
	FTimerHandle ExplosionTimerHandle;
	
	
};
