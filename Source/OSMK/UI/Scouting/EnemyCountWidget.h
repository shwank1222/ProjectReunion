#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyCountWidget.generated.h"

class UTextBlock;
class AOSMKGameState;

UCLASS()
class OSMK_API UEnemyCountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void UpdateEnemyCount();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_EnemyCount = nullptr;

private:
	TWeakObjectPtr<AOSMKGameState> CachedGameState = nullptr;
	int32 TotalEnemyCount = 0;
};