// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponents/WeaponComponents/WeaponBehaviour.h"
#include "ActorComponents/WeaponComponent.h"
#include "FirePattern.generated.h"



UCLASS(Abstract, Blueprintable, EditInlineNew, CollapseCategories, Config = Weapon, defaultconfig, configdonotcheckdefaults)
class VANAGANDR_API UFirePattern : public UWeaponBehaviour
{
	GENERATED_BODY()
	

public:
	virtual bool CheckFirePattern(EInputState inputState) {
		return false;
	}
};

UCLASS(NotBlueprintable, meta = (DisplayName = "SingleFire"))
class VANAGANDR_API USingleFirePattern : public UFirePattern
{
	GENERATED_BODY()

public:
	bool CheckFirePattern(EInputState inputState) override {
		if (inputState == EInputState::EIS_Started) {
			return true;
		}
		else return false;
	}
};

UCLASS(NotBlueprintable, meta = (DisplayName = "AutomaticFire"))
class VANAGANDR_API UAutomaticFirePattern : public UFirePattern
{
	GENERATED_BODY()

public:
	bool CheckFirePattern(EInputState inputState) override {
		if (inputState == EInputState::EIS_Stopped)
			return false;
		else {
			return true;
		}
	}
};


UCLASS(NotBlueprintable, meta = (DisplayName = "BurstFire"))
class VANAGANDR_API UBurstFirePattern : public UFirePattern
{
	GENERATED_BODY()

public:
	bool CheckFirePattern(EInputState inputState) override {
		if (inputState == EInputState::EIS_Started) {
			bShouldFire = true;
			fFireTimer = TimeBetweenFire;
			CurrentBurstFireCount = BurstFireCount;

			//this->Controller = Controller;
			//this->ShootingCharacter = ShootingCharacter;

			return true;
		}
		else if (inputState == EInputState::EIS_Ongoing) {

			if (fFireTimer <= 0 && CurrentBurstFireCount >= 0) {
				CurrentBurstFireCount--;
				fFireTimer = TimeBetweenFire;
				return true;
			}
			bShouldFire = false;
			return false;
		}
		
		return false;
	}

	void Tick(float DeltaTime, UWeaponComponent* weapon) override {
		Super::Tick(DeltaTime, weapon);

		if (!bShouldFire)
			return;

		if (fFireTimer > 0) {
			fFireTimer -= DeltaTime;
		}
	};

public:
	UPROPERTY(EditAnywhere)
	int BurstFireCount;
	UPROPERTY(EditAnywhere)
	float TimeBetweenFire;

private:
	bool bShouldFire;
	float fFireTimer;
	int CurrentBurstFireCount;

private:
	APlayerController* Controller;
	ACharacter* ShootingCharacter;
};