#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "OSMKGameState.generated.h"

UENUM(BlueprintType)
enum class EOSMKStageState : uint8
{
	Scouting    UMETA(DisplayName = "Scouting"),
	InProgress  UMETA(DisplayName = "InProgress"),
	Clear       UMETA(DisplayName = "Clear"),
	Failed      UMETA(DisplayName = "Failed"),
	Recap       UMETA(DisplayName = "Recap")
};

UCLASS()
class OSMK_API AOSMKGameState : public AGameState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void EndScoutingPhase();
	
public:
	UPROPERTY(BlueprintReadOnly)
	EOSMKStageState CurrentStageState = EOSMKStageState::Scouting;
};
