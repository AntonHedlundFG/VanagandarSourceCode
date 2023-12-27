// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPointManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/KismetSystemLibrary.h"

ASpawnPointManager::ASpawnPointManager()
{
	bReplicates = false;
}

void ASpawnPointManager::BeginPlay()
{
	Super::BeginPlay();

	//This actor is only used on the server.
	if (GetNetMode() >= ENetMode::NM_Client)
	{
		GetWorld()->DestroyActor(this);
		return;
	}

	LocatePlayerStarts();
}

void ASpawnPointManager::LocatePlayerStarts()
{
	//Find all PlayerStarts
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), OutActors);
	for (AActor* Actor : OutActors)
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(Actor);
		if (PlayerStart->PlayerStartTag == AttackerTag)
		{
			AttackerStarts.Add(PlayerStart);
		}
		else if (PlayerStart->PlayerStartTag == DefenderTag)
		{
			DefenderStarts.Add(PlayerStart);
		}
	}

}

APlayerStart* ASpawnPointManager::GetNextPlayerStart(bool bIsAttacker, TSubclassOf<APawn> PawnClass)
{
	TArray<APlayerStart*>& Starts = (bIsAttacker ? AttackerStarts : DefenderStarts);
	if (Starts.Num() == 0) return nullptr;

	int& NextIndex = (bIsAttacker ? NextAttackerIndex : NextDefenderIndex);
	int i = NextIndex % Starts.Num();

	//Loop around the Starts array, starting at NextIndex, to find an unoccupied spot.
	//If all starts are occupied, we simply return the one we started on and accept our fate.
	//We leave the NextIndex to be whatever index we return + 1;
	do {
		if (!IsStartOccupied(Starts[i], PawnClass))
			break;
		i = (i + 1) % Starts.Num();
	} while (i != NextIndex && i < Starts.Num()); //i < starts.Num() should protect us against 
	
	NextIndex = (i + 1) % Starts.Num();
	return Starts[i];
}

bool ASpawnPointManager::IsStartOccupied(APlayerStart* Start, TSubclassOf<APawn> PawnClass)
{
	if (!Start || !PawnClass) return false;

	return GetWorld()->EncroachingBlockingGeometry(
		PawnClass->GetDefaultObject<APawn>(),
		Start->GetActorLocation(), 
		Start->GetActorRotation());
}
