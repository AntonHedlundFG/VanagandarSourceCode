// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "VanagandrPlayerState.generated.h"

UENUM(BlueprintType)
enum class ETeam : uint8
{
	T_NONE = 0		UMETA(Hidden),
	T_TEAM_A = 1	UMETA(DisplayName = "Team A"),
	T_TEAM_B = 2	UMETA(DisplayName = "Team B")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamChange, ETeam, NewTeam);

class ACharacter;

/**
 * 
 */
UCLASS()
class VANAGANDR_API AVanagandrPlayerState : public APlayerState
{
	GENERATED_BODY()
	

public:
	UPROPERTY(BlueprintAssignable)
	FOnTeamChange OnTeamChange;

	UFUNCTION(BlueprintCallable)
	ETeam GetCurrentTeam() { return CurrentTeam; }

	UFUNCTION(BlueprintCallable)
	ETeam GetEnemyTeam();

	UFUNCTION(BlueprintCallable)
	bool IsAttacker();

	UFUNCTION(BlueprintCallable)
	bool IsDefender();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_TrySetCurrentTeam(ETeam NewTeam);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool IsDead();

	UFUNCTION(BlueprintCallable)
	bool IsAlive() { return !IsDead(); }

	//This is the character that the player has, we're keeping track of it
	//since we unpossess and re-possess it on death/respawn.
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<ACharacter> PlayerCharacter;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CurrentTeam)
	ETeam CurrentTeam;

	UFUNCTION()
	void OnRep_CurrentTeam();

protected:
	void BeginPlay() override;

};
