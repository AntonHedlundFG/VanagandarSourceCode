// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponents/WeaponComponents/WeaponBehaviour.h"
#include "BulletModifier.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, CollapseCategories, Config = Weapon, defaultconfig, configdonotcheckdefaults)
class VANAGANDR_API UBulletModifier : public UWeaponBehaviour
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Shooting")
	FVector ModifyBullet(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter, FVector AimDirection);

	virtual FVector ModifyBullet_Implementation(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter, FVector AimDirection) { return AimDirection; };
};
