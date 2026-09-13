#include "GameMode/OSMKInGameGameMode.h"

#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"
#include "Character/OSMKPlayerController.h"
#include "UI/Scouting/ScoutingWidget.h"
#include "UI/Ingame/OSMKIngameHUD.h"
#include "Core/OSMKGameState.h"
#include "Data/StageData.h"
#include "Data/Stage/StageExtractRule.h"
#include "Data/Stage/StageExtractedCache.h"
#include "Character/AI/EnemyCharacter.h"
#include "Character/PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "UI/StageResult/StageClearWidget.h"
#include "UI/Credits/CreditsWidget.h"
#include "Kismet/GameplayStatics.h"

void AOSMKInGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	CurrentStageIndex = 0;
	SpawnStage(CurrentStageIndex);

	if (!ScoutingWidgetClass)
	{
		return;
	}

	ScoutingWidget = CreateWidget<UScoutingWidget>(GetWorld(), ScoutingWidgetClass);
	if (ScoutingWidget)
	{
		ScoutingWidget->AddToViewport();
	}
}

void AOSMKInGameGameMode::SpawnStage(int32 StageIndex)
{
	ClearStage();

	CurrentStageIndex = StageIndex;
	CurrentRuleIndex = 0;
	PendingAsyncSpawns = 0;

	if (!StageData || !StageData->StageConfigs.IsValidIndex(StageIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] SpawnStage: invalid StageIndex %d"), StageIndex);
		return;
	}

	CurrentLevelName = StageData->StageConfigs[StageIndex].StageRowName;
	LoadedCache = StageData->Cache.LoadSynchronous();

	if (!LoadedCache)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] SpawnStage: Cache is null"));
		return;
	}

	AdvanceRules();
}

void AOSMKInGameGameMode::AdvanceRules()
{
	if (bAdvancingRules)
	{
		return;
	}

	if (!StageData)
	{
		OnAllRulesFinished();
		return;
	}

	bAdvancingRules = true;

	const TArray<TObjectPtr<UStageExtractRule>>& Rules = StageData->Rules;

	while (CurrentRuleIndex < Rules.Num() && PendingAsyncSpawns <= 0)
	{
		UStageExtractRule* Rule = Rules[CurrentRuleIndex];
		CurrentRuleIndex++;

		if (!Rule)
		{
			continue;
		}

		FStageSpawnContext Ctx;
		Ctx.World = GetWorld();
		Ctx.LevelName = CurrentLevelName;
		Ctx.Cache = LoadedCache;

		Ctx.TrackActor = [this](AActor* Actor)
		{
			if (IsValid(Actor))
			{
				SpawnedActors.Add(Actor);
			}
		};

		Ctx.TrackStreaming = [this](ULevelStreamingDynamic* Streaming)
		{
			if (!IsValid(Streaming))
			{
				return;
			}
			SpawnedLevelStreamings.Add(Streaming);
			Streaming->OnLevelLoaded.AddDynamic(this, &AOSMKInGameGameMode::OnLevelInstanceLoaded);
		};

		Ctx.SetPlayerStart = [this](const FTransform& T)
		{
			PlayerStartTransform = T;
		};

		Ctx.SetScoutCamera = [this](AActor* Cam)
		{
			SpawnedScoutCameraActor = Cam;
			SetupScoutCamera(Cam);
		};

		Ctx.BeginAsync = [this]()
		{
			PendingAsyncSpawns++;
		};

		Ctx.EndAsync = [this]()
		{
			PendingAsyncSpawns--;
			if (!bAdvancingRules)
			{
				AdvanceRules();
			}
		};

		Rule->SpawnStage(Ctx);
	}

	bAdvancingRules = false;

	if (CurrentRuleIndex >= Rules.Num() && PendingAsyncSpawns <= 0)
	{
		OnAllRulesFinished();
	}
}

void AOSMKInGameGameMode::OnLevelInstanceLoaded()
{
	PendingAsyncSpawns--;

	if (!bAdvancingRules)
	{
		AdvanceRules();
	}
}

void AOSMKInGameGameMode::OnAllRulesFinished()
{
	SpawnPlayerCharacter();

	if (AOSMKGameState* GS = GetGameState<AOSMKGameState>())
	{
		int32 EnemyCount = 0;
		for (AActor* Actor : SpawnedActors)
		{
			if (Cast<AEnemyCharacter>(Actor))
			{
				EnemyCount++;
			}
		}
		GS->SetEnemyCount(EnemyCount);
	}

	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
		{
			NavSys->Build();
		}
	});
}

void AOSMKInGameGameMode::SetupScoutCamera(AActor* CameraActor)
{
	if (!IsValid(CameraActor))
	{
		return;
	}

	GetWorldTimerManager().SetTimerForNextTick([this, CameraActor]()
	{
		if (!IsValid(CameraActor))
		{
			return;
		}
		if (AOSMKPlayerController* PC = Cast<AOSMKPlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			PC->SetViewTarget(CameraActor);
			PC->EnterScoutingMode(CameraActor, ScoutingWidget);
		}
	});
}

void AOSMKInGameGameMode::ClearStage()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	SpawnedActors.Empty();

	for (ULevelStreamingDynamic* Streaming : SpawnedLevelStreamings)
	{
		if (IsValid(Streaming))
		{
			Streaming->SetIsRequestingUnloadAndRemoval(true);
		}
	}
	SpawnedLevelStreamings.Empty();

	if (IsValid(SpawnedScoutCameraActor))
	{
		SpawnedScoutCameraActor->Destroy();
	}
	SpawnedScoutCameraActor = nullptr;

	if (IsValid(SpawnedPlayerCharacter))
	{
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			PC->UnPossess();
		}
		SpawnedPlayerCharacter->Destroy();
		SpawnedPlayerCharacter = nullptr;
	}

	PlayerStartTransform = FTransform::Identity;
	PendingAsyncSpawns = 0;
	CurrentRuleIndex = 0;
}

void AOSMKInGameGameMode::SpawnPlayerCharacter()
{
	if (!StageData)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] SpawnPlayerCharacter: StageData is null"));
		return;
	}

	UClass* PlayerCharacterClass = StageData->PlayerCharacterClass.LoadSynchronous();
	if (!PlayerCharacterClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] SpawnPlayerCharacter: PlayerCharacterClass is null"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	SpawnedPlayerCharacter = GetWorld()->SpawnActor<APawn>(PlayerCharacterClass, PlayerStartTransform, SpawnParams);
}

void AOSMKInGameGameMode::PossessPlayerCharacter()
{
	if (!SpawnedPlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[InGameGameMode] PossessPlayerCharacter: SpawnedPlayerCharacter is null"));
		return;
	}

	AOSMKPlayerController* PC = Cast<AOSMKPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC)
	{
		return;
	}

	PC->ExitScoutingMode();
	PC->Possess(SpawnedPlayerCharacter);

	if (APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(SpawnedPlayerCharacter))
	{
		if (AOSMKGameState* GS = GetGameState<AOSMKGameState>())
		{
			PlayerChar->OnCharacterDeath.AddUniqueDynamic(GS, &AOSMKGameState::PlayerDeath);
		}
	}

	if (ScoutingWidget)
	{
		ScoutingWidget->RemoveFromParent();
		ScoutingWidget = nullptr;
	}
}

void AOSMKInGameGameMode::ActivateEnemies()
{
	int32 ActivatedCount = 0;
	for (AActor* Actor : SpawnedActors)
	{
		if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Actor))
		{
			Enemy->ActivateEnemy();
			ActivatedCount++;
		}
	}
}

void AOSMKInGameGameMode::HandleStageClear()
{
	AOSMKGameState* GS = GetGameState<AOSMKGameState>();
	if (GS && GS->CurrentStageState != EOSMKStageState::InProgress)
	{
		return;
	}

	if (GS)
	{
		GS->CurrentStageState = EOSMKStageState::Clear;
	}

	GetWorldTimerManager().SetTimer(StageResultTimerHandle, this, &AOSMKInGameGameMode::ShowStageClearWidget, 1.5f, false);
}

void AOSMKInGameGameMode::HandleStageFail()
{
	AOSMKGameState* GS = GetGameState<AOSMKGameState>();
	if (GS && GS->CurrentStageState != EOSMKStageState::InProgress)
	{
		return;
	}

	if (GS)
	{
		GS->CurrentStageState = EOSMKStageState::Failed;
	}

	if (AOSMKIngameHUD* HUD = Cast<AOSMKIngameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		HUD->SetHUDVisible(false);
	}

	GetWorldTimerManager().SetTimer(StageResultTimerHandle, this, &AOSMKInGameGameMode::ShowStageFailWidget, 1.5f, false);
}

void AOSMKInGameGameMode::ShowStageClearWidget()
{
	if (StageClearWidgetClass)
	{
		StageClearWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), StageClearWidgetClass);
		if (StageClearWidgetInstance)
		{
			StageClearWidgetInstance->AddToViewport();
		}
	}

	if (AOSMKIngameHUD* HUD = Cast<AOSMKIngameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		HUD->SetHUDVisible(false);
	}

	GetWorldTimerManager().SetTimer(StageResultTimerHandle, this, &AOSMKInGameGameMode::ProceedToNextStage, 2.0f, false);
}

void AOSMKInGameGameMode::ShowStageFailWidget()
{
	if (StageFailWidgetClass)
	{
		if (UUserWidget* FailWidget = CreateWidget<UUserWidget>(GetWorld(), StageFailWidgetClass))
		{
			FailWidget->AddToViewport();

			if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
			{
				PC->SetShowMouseCursor(true);
				PC->SetInputMode(FInputModeUIOnly());
			}
		}
	}
}

void AOSMKInGameGameMode::ShowCredits()
{
	if (!CreditsWidgetClass)
	{
		return;
	}

	UCreditsWidget* Widget = CreateWidget<UCreditsWidget>(GetWorld(), CreditsWidgetClass);
	if (!Widget)
	{
		return;
	}

	Widget->bFastForwardOnInput = true;
	Widget->OnCreditsFinished.AddLambda([this]()
	{
		UGameplayStatics::OpenLevel(this, TitleLevelName);
	});
	Widget->AddToViewport();

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeUIOnly());
	}
}

void AOSMKInGameGameMode::ProceedToNextStage()
{
	if (IsValid(StageClearWidgetInstance))
	{
		if (UStageClearWidget* ClearWidget = Cast<UStageClearWidget>(StageClearWidgetInstance))
		{
			ClearWidget->PlayFadeOut();
		}
		else
		{
			StageClearWidgetInstance->RemoveFromParent();
		}
		StageClearWidgetInstance = nullptr;
	}

	CurrentStageIndex++;

	if (!StageData || !StageData->StageConfigs.IsValidIndex(CurrentStageIndex))
	{
		ShowCredits();
		return;
	}

	SpawnStage(CurrentStageIndex);

	if (ScoutingWidgetClass)
	{
		ScoutingWidget = CreateWidget<UScoutingWidget>(GetWorld(), ScoutingWidgetClass);
		if (ScoutingWidget)
		{
			ScoutingWidget->AddToViewport();
		}
	}
}

void AOSMKInGameGameMode::RetryStage()
{
	if (AOSMKGameState* GS = GetGameState<AOSMKGameState>())
	{
		GS->ResetStageState();
	}

	SpawnStage(CurrentStageIndex);

	if (ScoutingWidgetClass)
	{
		ScoutingWidget = CreateWidget<UScoutingWidget>(GetWorld(), ScoutingWidgetClass);
		if (ScoutingWidget)
		{
			ScoutingWidget->AddToViewport();
		}
	}
}
