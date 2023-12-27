// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VanagandrPlayerController.generated.h"

class ACharacter;
struct FBulletResults;

/**
 * 
 */
UCLASS()
class VANAGANDR_API AVanagandrPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Client, Reliable)
	void Client_DisconnectFromServer();

	UFUNCTION()
	void ApplyDamage(FBulletResults& Results);

protected:
	virtual void OnNetCleanup(UNetConnection* Connection) override;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ApplyDamage(AActor* DamagedActor, float BaseDamage, FVector HitFromDirection, FHitResult HitInfo);

};
