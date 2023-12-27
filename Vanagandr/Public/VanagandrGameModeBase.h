// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VanagandrGameModeBase.generated.h"

class AVanagandrGameState;
enum class EGamePhase : uint8;
enum class EBombState : uint8;
struct FKillInformation;

/**
 * 
 */
UCLASS()
class VANAGANDR_API AVanagandrGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	UFUNCTION()
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void BeginPlay() override;

protected:

	UFUNCTION()
	void ReadOptions();

	//Server player should call this manually via UI
	UFUNCTION(BlueprintCallable)
	void StartMatch();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RoundDurationTime = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MatchStartTime = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TeamSize = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RoundsPerHalftime = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BuyTime = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PostRoundTime = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PostGameTime = 30.0f;	

	UFUNCTION()
	void OnPhaseChange(EGamePhase NewPhase);

	UPROPERTY()
	EGamePhase CurrentPhase;

	UFUNCTION()
	void StartRound();

	UFUNCTION()
	void ResetAllResettables();

	UFUNCTION()
	void RemoveBindings();

	UFUNCTION(BlueprintCallable)
	void OnPlayerDeath(FKillInformation Info);

	UFUNCTION(BlueprintCallable)
	void OnBombStateChange(EBombState NewState);

	UFUNCTION()
	void CheckIfRoundOver();

	UFUNCTION()
	void EndGameIfBombUnplanted();

	UFUNCTION()
	void PostRoundLogic();
};
