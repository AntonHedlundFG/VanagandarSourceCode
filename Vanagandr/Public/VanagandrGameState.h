// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "VanagandrPlayerState.h"
#include "ActorComponents/WeaponComponent.h" //We might wanna move some enums to their own file instead of having to include the entire weapon
#include "VanagandrGameState.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	GP_NONE = 0			UMETA(Hidden),
	GP_WARMUP = 1		UMETA(DisplayName = "Warmup"),
	GP_BUY = 2			UMETA(DisplayName = "Buy"),
	GP_PLAY = 3			UMETA(DisplayName = "Play"),
	GP_POSTROUND = 4	UMETA(DisplayName = "PostRound"),
	GP_POSTGAME = 5		UMETA(DisplayName = "PostGame")
};

UENUM(BlueprintType)
enum class EBombState : uint8
{
	BS_UNPLANTED = 0	UMETA(DisplayName = "Unplanted"),
	BS_PLANTED = 1		UMETA(DisplayName = "Planted"),
	BS_DISARMED = 2		UMETA(DisplayName = "Disarmed"),
	BS_DETONATED = 3	UMETA(DisplayName = "Detonated")
};

class AVanagandrPlayerState;

USTRUCT(BlueprintType)
struct FRoundResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ETeam WinningTeam = ETeam::T_NONE;

	UPROPERTY(BlueprintReadOnly)
	EBombState BombState = EBombState::BS_UNPLANTED;

	FString ToString()
	{
		FString TeamName = WinningTeam == ETeam::T_TEAM_A ? FString("A") : FString("B");
		FString BombStateName;
		switch (BombState)
		{
		case EBombState::BS_UNPLANTED:
			BombStateName = FString("Unplanted");
			break;
		case EBombState::BS_PLANTED:
			BombStateName = FString("Planted");
			break;
		case EBombState::BS_DISARMED:
			BombStateName = FString("Disarmed");
			break;
		case EBombState::BS_DETONATED:
			BombStateName = FString("Detonated");
			break;
		}
		return FString("Winner: ").Append(TeamName).Append(FString(".   Bomb State: ")).Append(BombStateName).Append(FString("."));
	}
};

USTRUCT(BlueprintType)
struct FKillInformation
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	AVanagandrPlayerState* Victim = nullptr;

	UPROPERTY(BlueprintReadWrite)
	AVanagandrPlayerState* Killer = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UWeaponComponent* Weapon = nullptr;

	UPROPERTY(BlueprintReadWrite)
	bool bWasHeadshot = false;

	/** Not sure if this is the correct type for a weapon? What should it be?
	*/
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EWeaponType> WeaponType = (EWeaponType)(0);

	bool WasTeamKill()
	{
		if (!Victim || !Killer) return false;

		return Victim->GetCurrentTeam() == Killer->GetCurrentTeam();
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChange, EGamePhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountdownTimerChange, float, TimeRemaining);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerTeamChange, ETeam, NewTeam, AVanagandrPlayerState*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDeath, FKillInformation, KillInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBombStateChange, EBombState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamWin, ETeam, WinningTeam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDelayedDelegateCallback);


/**
 * 
 */
UCLASS()
class VANAGANDR_API AVanagandrGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable)
	FOnTeamWin OnTeamWin;

	//This is called whenever the server sets a new bomb state during a match.
	UPROPERTY(BlueprintAssignable)
	FOnBombStateChange OnBombStateChange;

	//This function should be used to plant and disarm the bomb. 
	//When resetting the level, don't call this, instead just change BombState directly.
	UFUNCTION(BlueprintCallable)
	void SetBombState(EBombState NewState);

	UFUNCTION(BlueprintCallable)
	EBombState GetBombState() { return BombState; }

	//Can be used to "gray out" the join-team UI button.
	UFUNCTION(BlueprintCallable)
	bool IsTeamFull(ETeam Team);

	UFUNCTION(BlueprintCallable)
	int GetTeamSize() { return TeamSize; }

	UFUNCTION()
	void SetTeamSize(int NewTeamSize) { TeamSize = NewTeamSize; }

	//This will be broadcast whenever a PlayerState joins a new team. 
	UPROPERTY(BlueprintAssignable)
	FOnPlayerTeamChange OnPlayerTeamChange;

	UFUNCTION(BlueprintCallable)
	TArray<AVanagandrPlayerState*> GetPlayersOfTeam(ETeam Team);

	UFUNCTION(BlueprintCallable)
	TArray<AVanagandrPlayerState*> GetAlivePlayersOfTeam(ETeam Team);

	UFUNCTION(BlueprintCallable)
	TArray<AVanagandrPlayerState*> GetDeadPlayersOfTeam(ETeam Team);

	//IndexIncrement is a value we can use to decide if we want to view next (1), previous (-1),
	//or just make sure we're not watching someone who's dead (0).
	UFUNCTION(BlueprintCallable)
	ACharacter* GetSpectateTarget(AVanagandrPlayerState* Observer, ACharacter* CurrentTarget, int IndexIncrement = 0);

	UFUNCTION(BlueprintCallable)
	TArray<AVanagandrPlayerState*> GetAllPlayers();

	//This one MUST be assigned in blueprints, and should be called on the server
	//any time a player dies.
	UPROPERTY(BlueprintAssignable)
	FOnPlayerDeath OnPlayerDeath;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Multicast_TriggerOnPlayerDeath(FKillInformation KillInfo);

	UFUNCTION(BlueprintCallable)
	ETeam GetAttackerTeam() { return AttackerTeam; }

	UFUNCTION(BlueprintCallable)
	ETeam GetDefenderTeam() { return DefenderTeam; }

	UFUNCTION(BlueprintCallable)
	int GetTeamScore(ETeam Team);

	UFUNCTION(BlueprintCallable)
	int GetStreak(ETeam Team, bool bWinStreak = true);

	UFUNCTION()
	void AddTeamWin(ETeam Team);

	UFUNCTION()
	void AddAttackerOrDefenderTeamWin(bool bAttackerTeamWins);

	UFUNCTION(BlueprintCallable)
	TArray<AVanagandrPlayerState*> GetAttackingPlayers() { return GetPlayersOfTeam(AttackerTeam); }

	UFUNCTION(BlueprintCallable)
	TArray<AVanagandrPlayerState*> GetDefendingPlayers() { return GetPlayersOfTeam(DefenderTeam); }

	UFUNCTION()
	void SwitchTeams();

	

	/** Returns a list of match wins, with the most recent match first in the list.
		The first round result will always be in the end of the list. */
	UFUNCTION(BlueprintCallable)
	TArray<FRoundResult> GetMatchWinHistory() { return MatchHistory; }

	UFUNCTION()
	void ClearMatchWinHistory();

	UFUNCTION()
	void StartNewRound();

	UFUNCTION(BlueprintCallable)
	int GetCurrentRound() { return CurrentRound; }
		
private:
	UPROPERTY(Replicated)
	ETeam AttackerTeam;

	UPROPERTY(Replicated)
	ETeam DefenderTeam;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_BombState)
	EBombState BombState;

	UFUNCTION()
	void OnRep_BombState();

	UPROPERTY(VisibleAnywhere, Replicated)
	int CurrentRound;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_MatchRoundHistory)
	TArray<FRoundResult> MatchHistory;

	UFUNCTION()
	void OnRep_MatchRoundHistory();

	UPROPERTY(VisibleAnywhere, Replicated)
	int TeamSize = 5;

	// Denotes which round was the last halftime team switch. Used for win streaks
	UPROPERTY(VisibleAnywhere, Replicated)
	int LastTeamSwitchRound = 0;

#pragma region GamePhase_Functions

public:

	//This is called when the server updates a timer which counts down to 
	//zero before the phase changes. If this is == -1.0f, no timer is set.
	UPROPERTY(BlueprintAssignable)
	FOnCountdownTimerChange OnCountdownTimerChange;

	UPROPERTY(BlueprintAssignable)
	FOnPhaseChange OnPhaseChange;

	//This simply sets the CurrentPhase and calls OnPhaseChange
	//Should not contain actual gameplay functionality, only notify
	//other listeners of changes.
	//Only works when called by server.
	UFUNCTION()
	void SetGamePhase(EGamePhase NewPhase);

	UFUNCTION()
	void SetGamePhaseDelayed(EGamePhase NewPhase, float Delay);

	UFUNCTION(BlueprintCallable)
	void CancelPhaseChangeTimer();

	UFUNCTION()
	void SetDelayedDelegateCallback(float Delay);
	UPROPERTY() FOnDelayedDelegateCallback OnDelayedDelegateCallback;
	UFUNCTION() void DoDelayedDelegateCallback();

	UFUNCTION(BlueprintCallable)
	EGamePhase GetGamePhase() { return CurrentPhase; }

	UFUNCTION(BlueprintCallable)
	float GetCountdownTimer() { return CountdownTimer; }
	
private:

	//Do not set this value manually, instead call SetGamePhase()
	//Otherwise events won't trigger properly.
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CurrentPhase)
	EGamePhase CurrentPhase;
	UFUNCTION() void OnRep_CurrentPhase();

	//Handle used for delayed phase changes, used in SetGamePhaseDelayed
	UPROPERTY()
	FTimerHandle SetGamePhaseHandle;

	//Do not set/get this manually. Use OnCountdownTimerChange delegate
	//or GetCountdownTimer()
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_DelayedPhaseChangeTimer)
	float CountdownTimer;
	UFUNCTION() void OnRep_DelayedPhaseChangeTimer();

#pragma endregion

};
