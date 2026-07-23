#pragma once

#include "GameFramework/Actor.h"
#include "GimmickBase.generated.h"

// Log Macros
DECLARE_LOG_CATEGORY_EXTERN(LogGimmick, Log, All);

#define GIMMICK_LOG(Verbosity, Format, ...) \
UE_LOG(LogGimmick, Verbosity, TEXT("[%s] " Format), *GetNameSafe(this), ##__VA_ARGS__)

UCLASS()
class OSMK_API AGimmickBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AGimmickBase();
	
	UFUNCTION(BlueprintCallable)
	void Trigger();
	
	virtual void OnTriggered() PURE_VIRTUAL(AGimmickBase::OnTriggered, );
	
	bool CanTrigger() const;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick")
	bool bTriggered = false;
};
