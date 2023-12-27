// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActorComponents/WeaponComponents/WeaponBehaviour.h"
#include "WeaponConstraint.generated.h"


class UWeapon;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, CollapseCategories, Config = Weapon, defaultconfig, configdonotcheckdefaults)
class VANAGANDR_API UWeaponConstraint : public UWeaponBehaviour
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Shooting")
	bool CheckConstraint();

	UFUNCTION()
	bool CheckConstraint_Implementation() {
		return true;
	};
};


UCLASS(NotBlueprintable, meta = (DisplayName = "Firerate"))
class VANAGANDR_API UFirerateConstraint : public UWeaponConstraint
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Firerate")
	float FireRate;
};

UCLASS(NotBlueprintable, meta = (DisplayName = "Magazine"))
class VANAGANDR_API UMagazineConstraint : public UWeaponConstraint
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Magazine")
	float MagazineSize;
};

