// Fill out your copyright notice in the Description page of Project Settings.


#include "VanagandrPlayerState.h"
#include "VanagandrGameState.h"
#include "VanagandrGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

void AVanagandrPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AVanagandrPlayerState, CurrentTeam);
	DOREPLIFETIME(AVanagandrPlayerState, PlayerCharacter);
}

void AVanagandrPlayerState::Server_TrySetCurrentTeam_Implementation(ETeam NewTeam)
{
	AVanagandrGameState* VGameState = GetWorld()->GetGameState<AVanagandrGameState>();
	if (VGameState && 
		//Commented this out as we want players to be able to join mid-game.
		//VGameState->GetGamePhase() == EGamePhase::GP_WARMUP && 
		!VGameState->IsTeamFull(NewTeam))
	{
		CurrentTeam = NewTeam;
		OnRep_CurrentTeam();
	}
}

ETeam AVanagandrPlayerState::GetEnemyTeam()
{
	switch (CurrentTeam)
	{
	case ETeam::T_TEAM_A:
		return ETeam::T_TEAM_B;
	case ETeam::T_TEAM_B:
		return ETeam::T_TEAM_A;
	default:
		return ETeam::T_NONE;
	}
}

bool AVanagandrPlayerState::IsAttacker()
{
	AVanagandrGameState* VGameState = GetWorld()->GetGameState<AVanagandrGameState>();
	return (VGameState && VGameState->GetAttackerTeam() == CurrentTeam);
}

bool AVanagandrPlayerState::IsDefender()
{
	AVanagandrGameState* VGameState = GetWorld()->GetGameState<AVanagandrGameState>();
	return (VGameState && VGameState->GetDefenderTeam() == CurrentTeam);
}

void AVanagandrPlayerState::OnRep_CurrentTeam()
{
	OnTeamChange.Broadcast(CurrentTeam);
	AVanagandrGameState* VGameState = GetWorld()->GetGameState<AVanagandrGameState>();
	if (VGameState)
		VGameState->OnPlayerTeamChange.Broadcast(CurrentTeam, this);
}

void AVanagandrPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UVanagandrGameInstance* VGameInstance = GetGameInstance<UVanagandrGameInstance>();
	if (VGameInstance && GetOwningController() && GetOwningController()->IsLocalPlayerController())
	{
		UKismetSystemLibrary::PrintString(this, 
			FString("Set player name to: ") + VGameInstance->GetPlayerUsername());
		SetPlayerName(VGameInstance->GetPlayerUsername());
	}
}
