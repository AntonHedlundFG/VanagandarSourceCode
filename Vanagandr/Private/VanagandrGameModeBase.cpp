// Fill out your copyright notice in the Description page of Project Settings.


#include "VanagandrGameModeBase.h"
#include "VanagandrGameInstance.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "GameFramework/OnlineReplStructs.h"
#include "VanagandrGameState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "VanagandrPlayerController.h"
#include "VanagandrPlayerState.h"
#include "VanagandrGameState.h"
#include "Interfaces/Resettable.h"
#include "PlayableMaps.h"


void AVanagandrGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	/* This handles the unique network ID for the player, which is required
	* to register a player joining for Epic Online Services. */
	if (NewPlayer)
	{
		FUniqueNetIdPtr UniqueNetId;
		FUniqueNetIdRepl UniqueNetIDRepl;
		if (NewPlayer->IsLocalController())
		{
			ULocalPlayer* LocalPlayer = NewPlayer->GetLocalPlayer();
			if (LocalPlayer)
			{
				UniqueNetIDRepl = LocalPlayer->GetPreferredUniqueNetId();
			}
			else
			{
				UNetConnection* RemoteNetConnection = Cast<UNetConnection>(NewPlayer->Player);
				check(IsValid(RemoteNetConnection));
				UniqueNetIDRepl = RemoteNetConnection->PlayerId;
			}
		}
		else
		{
			UNetConnection* RemoteNetConnection = Cast<UNetConnection>(NewPlayer->Player);
			check(IsValid(RemoteNetConnection));
			UniqueNetIDRepl = RemoteNetConnection->PlayerId;
		}

		UniqueNetId = UniqueNetIDRepl.GetUniqueNetId();
		if (UniqueNetId.IsValid())
		{
			IOnlineSubsystem* Subsystem = Online::GetSubsystem(NewPlayer->GetWorld());
			IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
			bool bRegistrationSuccess = Session->RegisterPlayer(SESSION_NAME, *UniqueNetId, false);
		}

		if (GetNumPlayers() > TeamSize * 2 && UniqueNetId)
		{
			IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
			if (!Subsystem) return;
			IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
			if (!Session) return;
			//PRINT(FString("Disconnect"))
			//Session->UnregisterPlayer(SESSION_NAME, *UniqueNetId.Get());
			//Session->RemovePlayerFromSession(0, SESSION_NAME, *UniqueNetId.Get());
			if (AVanagandrPlayerController* CastController = Cast<AVanagandrPlayerController>(NewPlayer))
			{
				CastController->Client_DisconnectFromServer();
			}
		}


	}


}

void AVanagandrGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	//Parse any options that were sent along the map load command as a parameter
	//like "?TeamSize=3"
	ReadOptions();

	
	
	AVanagandrGameState* State = GetGameState<AVanagandrGameState>();
	if (State)
	{
		//Update settings
		State->SetTeamSize(TeamSize);

		//Game should start in warmup phase.
		State->OnPhaseChange.AddDynamic(this, &AVanagandrGameModeBase::OnPhaseChange);
		State->SetGamePhase(EGamePhase::GP_WARMUP);
	}

}

void AVanagandrGameModeBase::ReadOptions()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem) return;
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (!Session) return;

	//We need this in case we're testing in editor without logging in.
	if (!Session->GetSessionSettings(SESSION_NAME)) return;

	//Cache current online settings
	FOnlineSessionSettings Settings = *Session->GetSessionSettings(SESSION_NAME);

	//Read options and update cached Settings
	if (UGameplayStatics::HasOption(OptionsString, TEAM_SIZE_OPTION))
	{
		TeamSize = UGameplayStatics::GetIntOption(OptionsString, TEAM_SIZE_OPTION, 5);
		Settings.NumPublicConnections = TeamSize * 2;
	}
	if (UGameplayStatics::HasOption(OptionsString, ROUNDS_PER_HALFTIME_OPTION))
	{
		RoundsPerHalftime = UGameplayStatics::GetIntOption(OptionsString, ROUNDS_PER_HALFTIME_OPTION, 6);
		Settings.Set(GAMES_HALFTIME, FString::FromInt(RoundsPerHalftime), EOnlineDataAdvertisementType::ViaOnlineService);
	}

	if (UGameplayStatics::HasOption(OptionsString, MAP_NAME))
	{
		FString MapName = UGameplayStatics::ParseOption(OptionsString, MAP_NAME);
		Settings.Set(LOADED_MAP_NAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);
	}

	//Update online session with changed properties
	Session->UpdateSession(SESSION_NAME, Settings);

}

void AVanagandrGameModeBase::StartMatch()
{
	AVanagandrGameState* State = GetGameState<AVanagandrGameState>();
	if (!State) return;

	//Can only run on servers
	if (GetNetMode() > ENetMode::NM_ListenServer) return;

	if (State->GetGamePhase() != EGamePhase::GP_WARMUP) return;

	State->SetGamePhaseDelayed(EGamePhase::GP_BUY, MatchStartTime);
	
	UVanagandrGameInstance* GameInstance = GetGameInstance<UVanagandrGameInstance>();
	if (GameInstance)
	{
		GameInstance->SetSessionInProgress(true);
	}
}

void AVanagandrGameModeBase::OnPhaseChange(EGamePhase NewPhase)
{
	AVanagandrGameState* State = GetGameState<AVanagandrGameState>();
	if (!State) return;

	RemoveBindings();

	UVanagandrGameInstance* GameInstance = GetGameInstance<UVanagandrGameInstance>();

	switch (NewPhase)
	{
	case EGamePhase::GP_WARMUP:
		State->ClearMatchWinHistory();
		if (GameInstance && GameInstance->IsSessionInProgress())
		{
			GameInstance->SetSessionInProgress(false);
		}
		break;
	case EGamePhase::GP_BUY:
		ResetAllResettables();
		State->StartNewRound();
		State->SetGamePhaseDelayed(EGamePhase::GP_PLAY, BuyTime);
		break;
	case EGamePhase::GP_PLAY:
		StartRound();
		CheckIfRoundOver();
		break;
	case EGamePhase::GP_POSTROUND:
		State->SetGamePhaseDelayed(EGamePhase::GP_BUY, PostRoundTime);
		PostRoundLogic();
		break;
	case EGamePhase::GP_POSTGAME:
		//Removed this line so we don't restart the game when it's over.
		//State->SetGamePhaseDelayed(EGamePhase::GP_WARMUP, PostGameTime);
		
		//It could be nice to have an automatic server shutdown timer here
		break;
	default:
		break;
	}
}

void AVanagandrGameModeBase::StartRound()
{
	AVanagandrGameState* State = GetGameState<AVanagandrGameState>();
	if (!State) return;

	//Check if a team has won whenever a player dies or the bomb changes state
	State->OnPlayerDeath.AddDynamic(this, &AVanagandrGameModeBase::OnPlayerDeath);
	State->OnBombStateChange.AddDynamic(this, &AVanagandrGameModeBase::OnBombStateChange);

	//Set a timer with a callback for the round duration. If time runs out, we check for the bomb
	//state, and end the round if appropriate.
	State->OnDelayedDelegateCallback.AddDynamic(this, &AVanagandrGameModeBase::EndGameIfBombUnplanted);
	State->SetDelayedDelegateCallback(RoundDurationTime);
}

void AVanagandrGameModeBase::ResetAllResettables()
{
	TArray<AActor*> Resettables;
	UGameplayStatics::GetAllActorsWithInterface(this, UResettable::StaticClass(), Resettables);
	for (AActor* Resettable : Resettables)
	{
		IResettable::Execute_VanagandrReset(Resettable);
	}
}

void AVanagandrGameModeBase::RemoveBindings()
{
	AVanagandrGameState* State = GetGameState<AVanagandrGameState>();
	if (!State) return;

	State->OnPlayerDeath.RemoveDynamic(this, &AVanagandrGameModeBase::OnPlayerDeath);
	State->OnBombStateChange.RemoveDynamic(this, &AVanagandrGameModeBase::OnBombStateChange);
	State->OnDelayedDelegateCallback.RemoveAll(this);
}

void AVanagandrGameModeBase::OnPlayerDeath(FKillInformation Info)
{
	CheckIfRoundOver();
}

void AVanagandrGameModeBase::OnBombStateChange(EBombState NewState)
{
	CheckIfRoundOver();
}

void AVanagandrGameModeBase::PostRoundLogic()
{
	AVanagandrGameState* State = GetGameState<AVanagandrGameState>();
	if (!State) return;


	if (State->GetTeamScore(ETeam::T_TEAM_A) > RoundsPerHalftime)
	{
		State->SetGamePhase(EGamePhase::GP_POSTGAME);
		return;
	}
	if (State->GetTeamScore(ETeam::T_TEAM_B) > RoundsPerHalftime)
	{
		State->SetGamePhase(EGamePhase::GP_POSTGAME);
		return;
	}

	if (State->GetCurrentRound() == RoundsPerHalftime)
	{
		State->SwitchTeams();
	}

}

void AVanagandrGameModeBase::EndGameIfBombUnplanted()
{
	AVanagandrGameState* State = GetGameState<AVanagandrGameState>();
	if (!State) return;

	if (State->GetGamePhase() == EGamePhase::GP_PLAY &&
		State->GetBombState() == EBombState::BS_UNPLANTED)
	{
		State->AddAttackerOrDefenderTeamWin(false);
		State->SetGamePhase(EGamePhase::GP_POSTROUND);
	}
}

void AVanagandrGameModeBase::CheckIfRoundOver()
{
	AVanagandrGameState* State = GetGameState<AVanagandrGameState>();
	if (!State) return;

	if (State->GetBombState() == EBombState::BS_DISARMED)
	{
		State->AddAttackerOrDefenderTeamWin(false);
		State->SetGamePhase(EGamePhase::GP_POSTROUND);
		return;
	}

	if (State->GetBombState() == EBombState::BS_DETONATED)
	{
		State->AddAttackerOrDefenderTeamWin(true);
		State->SetGamePhase(EGamePhase::GP_POSTROUND);
		return;
	}

	bool bAttackersAreDead = true;
	for (AVanagandrPlayerState* Player : State->GetAttackingPlayers())
	{
		if (!Player->IsDead())
		{
			bAttackersAreDead = false;
			break;
		}
	}
	if (bAttackersAreDead && State->GetBombState() != EBombState::BS_PLANTED)
	{
		State->AddAttackerOrDefenderTeamWin(false);
		State->SetGamePhase(EGamePhase::GP_POSTROUND);
		return;
	}

	bool bDefendersAreDead = true;
	for (AVanagandrPlayerState* Player : State->GetDefendingPlayers())
	{
		if (!Player->IsDead())
		{
			bDefendersAreDead = false;
			break;
		}
	}
	if (bDefendersAreDead)
	{
		State->AddAttackerOrDefenderTeamWin(true);
		State->SetGamePhase(EGamePhase::GP_POSTROUND);
		return;
	}

}