// Fill out your copyright notice in the Description page of Project Settings.


#include "VanagandrGameState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "VanagandrGameModeBase.h"
#include "VanagandrGameInstance.h"
#include "Net/UnrealNetwork.h"

void AVanagandrGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AVanagandrGameState, CurrentPhase);
	DOREPLIFETIME(AVanagandrGameState, CountdownTimer);
	DOREPLIFETIME(AVanagandrGameState, AttackerTeam);
	DOREPLIFETIME(AVanagandrGameState, DefenderTeam);
	DOREPLIFETIME(AVanagandrGameState, BombState);
	DOREPLIFETIME(AVanagandrGameState, CurrentRound);
	DOREPLIFETIME(AVanagandrGameState, MatchHistory);
	DOREPLIFETIME(AVanagandrGameState, TeamSize);
	DOREPLIFETIME(AVanagandrGameState, LastTeamSwitchRound);
}

void AVanagandrGameState::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(true);

	if (GetNetMode() <= ENetMode::NM_ListenServer)
	{
		AttackerTeam = ETeam::T_TEAM_A;
		DefenderTeam = ETeam::T_TEAM_B;
		CurrentRound = 0;
	}
}

void AVanagandrGameState::SetBombState(EBombState NewState)
{
	//Can only run on servers
	if (GetNetMode() > ENetMode::NM_ListenServer) return;

	if (BombState == NewState) return;

	BombState = NewState;
	OnRep_BombState();
}

void AVanagandrGameState::OnRep_MatchRoundHistory()
{
	if (MatchHistory.Num() > 0)
		OnTeamWin.Broadcast(MatchHistory[0].WinningTeam);
}

void AVanagandrGameState::OnRep_BombState()
{
	OnBombStateChange.Broadcast(BombState);
}

void AVanagandrGameState::ClearMatchWinHistory()
{
	//Can only run on servers
	if (GetNetMode() > ENetMode::NM_ListenServer) return;

	MatchHistory.Empty();
}

void AVanagandrGameState::StartNewRound()
{
	CurrentRound++;
	BombState = EBombState::BS_UNPLANTED;
}

void AVanagandrGameState::Tick(float DeltaTime)
{

	//Can only run on servers
	if (GetNetMode() > ENetMode::NM_ListenServer) return;

	//Set CountdownTimer to the current time remaining of the handle
	//and call onrep function if a change has happened.


float OldTimerValue = CountdownTimer;
CountdownTimer = GetWorld()->GetTimerManager().GetTimerRemaining(SetGamePhaseHandle);
if (OldTimerValue != CountdownTimer)
OnRep_DelayedPhaseChangeTimer();
}

bool AVanagandrGameState::IsTeamFull(ETeam Team)
{
	int PlayerStates = UGameplayStatics::GetNumPlayerStates(this);
	int TeamCount = 0;
	for (int i = 0; i < PlayerStates; i++)
	{
		APlayerState* PS = UGameplayStatics::GetPlayerState(this, i);
		AVanagandrPlayerState* VPS = Cast<AVanagandrPlayerState>(PS);
		if (VPS && VPS->GetCurrentTeam() == Team)
		{
			TeamCount++;
		}
	}

	return TeamCount >= TeamSize;
}


TArray<AVanagandrPlayerState*> AVanagandrGameState::GetPlayersOfTeam(ETeam Team)
{
	TArray<AVanagandrPlayerState*> List;

	int PlayerStates = UGameplayStatics::GetNumPlayerStates(this);
	for (int i = 0; i < PlayerStates; i++)
	{
		APlayerState* PS = UGameplayStatics::GetPlayerState(this, i);
		AVanagandrPlayerState* VPS = Cast<AVanagandrPlayerState>(PS);
		if (VPS && VPS->GetCurrentTeam() == Team)
		{
			List.Add(VPS);
		}
	}

	return List;
}

TArray<AVanagandrPlayerState*> AVanagandrGameState::GetAlivePlayersOfTeam(ETeam Team)
{
	return GetPlayersOfTeam(Team).FilterByPredicate([&](AVanagandrPlayerState* State) {return !State->IsDead();});
}

TArray<AVanagandrPlayerState*> AVanagandrGameState::GetDeadPlayersOfTeam(ETeam Team)
{
	return GetPlayersOfTeam(Team).FilterByPredicate([&](AVanagandrPlayerState* State) {return State->IsDead();});
}

ACharacter* AVanagandrGameState::GetSpectateTarget(AVanagandrPlayerState* Observer, ACharacter* CurrentTarget, int IndexIncrement)
{
	auto ValidTargets = GetAlivePlayersOfTeam(Observer->GetCurrentTeam());
	if (ValidTargets.Num() == 0)
		return (CurrentTarget == nullptr ? Observer->PlayerCharacter : CurrentTarget);
	if (CurrentTarget == nullptr)
		return ValidTargets[0]->PlayerCharacter;
	int CurrentTargetIndex = ValidTargets.IndexOfByPredicate([&](AVanagandrPlayerState* State) {return State->PlayerCharacter == CurrentTarget;});
	if (CurrentTargetIndex == INDEX_NONE)
		CurrentTargetIndex = 0;
	int NewTargetIndex = (CurrentTargetIndex + IndexIncrement) % ValidTargets.Num();
	while (NewTargetIndex < 0)
		NewTargetIndex += ValidTargets.Num();
	return ValidTargets[NewTargetIndex]->PlayerCharacter;
}

TArray<AVanagandrPlayerState*> AVanagandrGameState::GetAllPlayers()
{
	TArray<AVanagandrPlayerState*> AllPlayers;
	AllPlayers.Append(GetPlayersOfTeam(ETeam::T_TEAM_A));
	AllPlayers.Append(GetPlayersOfTeam(ETeam::T_TEAM_B));
	return AllPlayers;
}

void AVanagandrGameState::Multicast_TriggerOnPlayerDeath_Implementation(FKillInformation KillInfo)
{
	OnPlayerDeath.Broadcast(KillInfo);
}

int AVanagandrGameState::GetTeamScore(ETeam Team)
{
	auto WonMatches = MatchHistory.FilterByPredicate([&](FRoundResult MatchHistory)
		{ return MatchHistory.WinningTeam == Team;});

	return WonMatches.Num();
}

int AVanagandrGameState::GetStreak(ETeam Team, bool bWinStreak)
{
	int StreakCounter = 0;

	//We don't count the matches after "LastTeamSwitchRound" since streaks don't count
	//between halftimes.
	for (int i = 0; i < MatchHistory.Num() - LastTeamSwitchRound; i++)
	{
		if ((MatchHistory[i].WinningTeam == Team) == bWinStreak)
		{
			StreakCounter++;
		}
		else
		{
			break;
		}
	}

	return StreakCounter;
}

void AVanagandrGameState::AddTeamWin(ETeam Team)
{
	//Can only run on servers
	if (GetNetMode() > ENetMode::NM_ListenServer) return;

	FRoundResult Round;
	Round.WinningTeam = Team;
	Round.BombState = GetBombState();

	if (Team >= ETeam::T_TEAM_A)
	{
		MatchHistory.Insert(Round, 0);
		OnRep_MatchRoundHistory();
	}

	int TeamAScore = GetTeamScore(ETeam::T_TEAM_A);
	int TeamBScore = GetTeamScore(ETeam::T_TEAM_B);

	auto GameInstance = GetGameInstance<UVanagandrGameInstance>();
	if (GameInstance)
	{
		GameInstance->UpdateSessionGameWins(TeamAScore, TeamBScore);
	}

}

void AVanagandrGameState::AddAttackerOrDefenderTeamWin(bool bAttackerTeamWins)
{
	ETeam Team = (bAttackerTeamWins ? GetAttackerTeam() : GetDefenderTeam());
	AddTeamWin(Team);
}

void AVanagandrGameState::SwitchTeams()
{
	//Can only run on servers
	if (GetNetMode() > ENetMode::NM_ListenServer) return;

	ETeam Temp = AttackerTeam;
	AttackerTeam = DefenderTeam;
	DefenderTeam = Temp;

	//We need to know last team switch round to reset win streaks
	LastTeamSwitchRound = MatchHistory.Num();
}

void AVanagandrGameState::SetGamePhase(EGamePhase NewPhase)
{
	//Can only run on servers
	if (GetNetMode() > ENetMode::NM_ListenServer) return;
	
	//Don't run if there's no change
	if (NewPhase == CurrentPhase) return;

	GetWorld()->GetTimerManager().ClearTimer(SetGamePhaseHandle);

	CurrentPhase = NewPhase;
	OnRep_CurrentPhase();

}

void AVanagandrGameState::SetGamePhaseDelayed(EGamePhase NewPhase, float Delay)
{
	//Can only run on servers
	if (GetNetMode() > ENetMode::NM_ListenServer) return;

	//Don't run if there's no change
	if (NewPhase == CurrentPhase) return;

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("SetGamePhase"), NewPhase);
	GetWorld()->GetTimerManager().SetTimer(SetGamePhaseHandle, TimerDelegate, Delay, false);
}

void AVanagandrGameState::CancelPhaseChangeTimer()
{
	//Can only run on servers
	if (GetNetMode() > ENetMode::NM_ListenServer) return;

	if (SetGamePhaseHandle.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(SetGamePhaseHandle);
}

void AVanagandrGameState::SetDelayedDelegateCallback(float Delay)
{
	//Can only run on servers
	if (GetNetMode() > ENetMode::NM_ListenServer) return;

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("DoDelayedDelegateCallback"));
	GetWorld()->GetTimerManager().SetTimer(SetGamePhaseHandle, TimerDelegate, Delay, false);
}

void AVanagandrGameState::DoDelayedDelegateCallback()
{
	OnDelayedDelegateCallback.Broadcast();
}

void AVanagandrGameState::OnRep_CurrentPhase()
{
	OnPhaseChange.Broadcast(CurrentPhase);
}

void AVanagandrGameState::OnRep_DelayedPhaseChangeTimer()
{
	OnCountdownTimerChange.Broadcast(CountdownTimer);
}
