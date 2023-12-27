// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponents/WeaponComponents/WeaponBehaviour.h"
#include "WeaponEffect.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, CollapseCategories, Config = Weapon, defaultconfig, configdonotcheckdefaults)
class VANAGANDR_API UWeaponEffect : public UWeaponBehaviour
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Shooting")
	void PlayEffect(FBulletResults& bulletResults);

protected:
	virtual void PlayEffect_Implementation(FBulletResults& bulletResults);
	
};
