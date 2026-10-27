#include "GameMode/OSMKInGameGameMode.h"

#include "NavigationSystem.h"
#include "NavigationPath.h"
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
#include "Core/OSMKBGMSubsystem.h"

void AOSMKInGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	CurrentStageIndex = 0;

	if (ScoutingWidgetClass)
	{
		ScoutingWidget = CreateWidget<UScoutingWidget>(GetWorld(), ScoutingWidgetClass);
		if (ScoutingWidget)
		{
			ScoutingWidget->AddToViewport();
		}
	}

	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		SpawnStage(CurrentStageIndex);
	}));
}

void AOSMKInGameGameMode::SpawnStage(int32 StageIndex)
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UOSMKBGMSubsystem* BGM = GI->GetSubsystem<UOSMKBGMSubsystem>())
		{
			BGM->SetCombatLayer(false);
		}
	}

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
		GS->NotifyStageSpawned(CurrentStageIndex, CurrentLevelName);
	}

	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
		{
			NavSys->Build();
		}

		AssignEnemyGroups();
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
	if (AOSMKGameState* GS = GetGameState<AOSMKGameState>())
	{
		GS->DestroyReplayCameraActor();
	}

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

void AOSMKInGameGameMode::AssignEnemyGroups()
{
	TArray<AEnemyCharacter*> Enemies;
	for (AActor* Actor : SpawnedActors)
	{
		if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Actor))
		{
			Enemy->SetAlertGroupId(INDEX_NONE);
			Enemies.Add(Enemy);
		}
	}

	const float RadiusSq = FMath::Square(EnemyGroupRadius);
	int32 NextGroupId = 0;

	auto IsWithinGroupPath = [this, RadiusSq](const AEnemyCharacter* A, const AEnemyCharacter* B)
	{
		if (FVector::DistSquared(A->GetActorLocation(), B->GetActorLocation()) > RadiusSq)
		{
			return false;
		}

		const UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), A->GetActorLocation(), B->GetActorLocation());
		return Path && Path->IsValid() && !Path->IsPartial() && Path->GetPathLength() <= EnemyGroupRadius;
	};

	for (AEnemyCharacter* Seed : Enemies)
	{
		if (Seed->GetAlertGroupId() != INDEX_NONE)
		{
			continue;
		}

		const int32 GroupId = NextGroupId++;
		Seed->SetAlertGroupId(GroupId);

		TArray<AEnemyCharacter*> Queue = { Seed };
		while (Queue.Num() > 0)
		{
			const AEnemyCharacter* Current = Queue.Pop();
			for (AEnemyCharacter* Other : Enemies)
			{
				if (Other->GetAlertGroupId() != INDEX_NONE)
				{
					continue;
				}

				if (IsWithinGroupPath(Current, Other))
				{
					Other->SetAlertGroupId(GroupId);
					Queue.Add(Other);
				}
			}
		}
	}
}

void AOSMKInGameGameMode::AlertEnemyGroup(int32 GroupId)
{
	if (GroupId == INDEX_NONE)
	{
		return;
	}

	for (AActor* Actor : SpawnedActors)
	{
		AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Actor);
		if (IsValid(Enemy) && Enemy->GetAlertGroupId() == GroupId)
		{
			Enemy->ForceAlert();
		}
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
	ShowStageClearWidget();
}

void AOSMKInGameGameMode::HandleStageFail()
{
	if (AOSMKIngameHUD* HUD = Cast<AOSMKIngameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		HUD->SetHUDVisible(false);
	}

	ShowStageFailWidget();
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

	GetWorldTimerManager().SetTimer(StageResultTimerHandle, this, &AOSMKInGameGameMode::ProceedToNextStage, 4.0f, false);
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
	if (AOSMKGameState* GS = GetGameState<AOSMKGameState>())
	{
		GS->DestroyReplayCameraActor();
	}

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
