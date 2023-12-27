// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponents/WeaponComponents/WeaponBehaviour.h"
#include "ActorComponents/WeaponComponent.h"
#include "BulletSpawner.generated.h"


UCLASS(Abstract, Blueprintable, EditInlineNew, CollapseCategories, Config = Weapon, defaultconfig, configdonotcheckdefaults)
class VANAGANDR_API UBulletSpawner : public UWeaponBehaviour
{

	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Shooting")
	FBulletResults CreateBullet(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter,
		FVector ShootWorldStartPoint, FVector AimDirection);

protected:
	virtual FBulletResults CreateBullet_Implementation(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter,
		FVector ShootWorldStartPoint, FVector AimDirection);
};
