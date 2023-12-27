// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "SpawnPointManager.generated.h"

#define PRINT(msg) UKismetSystemLibrary::PrintString(this, msg);

class APlayerStart;

/**
 * 
 */
UCLASS()
class VANAGANDR_API ASpawnPointManager : public AInfo
{
	GENERATED_BODY()

protected: 
	ASpawnPointManager();
	virtual void BeginPlay() override;

	//Finds all PlayerStarts and matches them to AttackerTag and DefenderTag
	//and stores them in the AttackerStarts and DefenderStarts arrays.
	UFUNCTION()
	void LocatePlayerStarts();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName AttackerTag = FName("Attacker");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName DefenderTag = FName("Defender");

	//We store lists of all applicable PlayerStarts, and keep track of which is
	//the next index we should use, so that we cycle through the spawns.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APlayerStart*> AttackerStarts;
	UPROPERTY() int NextAttackerIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APlayerStart*> DefenderStarts;
	UPROPERTY() int NextDefenderIndex = 0;

public:

	//Will return an appropriate PlayerStart, prioritizing unoccupied spots
	UFUNCTION(BlueprintCallable)
	APlayerStart* GetNextPlayerStart(bool bIsAttacker, TSubclassOf<APawn> PawnClass);

	UFUNCTION(BlueprintCallable)
	bool IsStartOccupied(APlayerStart* Start, TSubclassOf<APawn> PawnClass);
	
};
