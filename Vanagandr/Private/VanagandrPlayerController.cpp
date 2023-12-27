// Fill out your copyright notice in the Description page of Project Settings.


#include "VanagandrPlayerController.h"
#include "VanagandrGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "ActorComponents/WeaponComponent.h"

void AVanagandrPlayerController::Client_DisconnectFromServer_Implementation()
{
	UVanagandrGameInstance* GameInstance = Cast<UVanagandrGameInstance>(GetWorld()->GetGameInstance());

	if (GameInstance)
	{
		ClientTravel(TEXT("/Game/Levels/MainMenu/MainMenu?closed"), ETravelType::TRAVEL_Absolute);
		GameInstance->DestroyEoSSession();
	}
}


void AVanagandrPlayerController::OnNetCleanup(UNetConnection* Connection)
{
	UVanagandrGameInstance* GameInstance = Cast<UVanagandrGameInstance>(GetWorld()->GetGameInstance());

	if (GameInstance)
	{
		GameInstance->DestroyEoSSession();
	}

	Super::OnNetCleanup(Connection);
}

void AVanagandrPlayerController::ApplyDamage(FBulletResults& Results)
{
	for (FHitResult HitResult : Results.HitResults)
	{
		//Get data and use Server_ApplyDamage();
		//Do NOT use Server_ApplyDamage_Implementation directly.
	}
}

void AVanagandrPlayerController::Server_ApplyDamage_Implementation(AActor* DamagedActor, float BaseDamage, FVector HitFromDirection, FHitResult HitInfo)
{
	UGameplayStatics::ApplyPointDamage(DamagedActor, BaseDamage, HitFromDirection, HitInfo, this, nullptr, nullptr);
}

bool AVanagandrPlayerController::Server_ApplyDamage_Validate(AActor* DamagedActor, float BaseDamage, FVector HitFromDirection, FHitResult HitInfo)
{
	return true;
}