// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponents/WeaponComponent.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "WeaponBehaviour.generated.h"

class UWeaponComponent;

UCLASS(Abstract, Blueprintable, EditInlineNew, CollapseCategories, Config = Weapon, defaultconfig, configdonotcheckdefaults)
class VANAGANDR_API UWeaponBehaviour : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, BlueprintReadOnly, Category = "Contraint Settings")
	bool bShouldTick = true;

public:
	virtual void Begin(TObjectPtr<UWeaponComponent> weapon, UWorld* world) {
		SetWorld(world);
		SetWeapon(weapon);
		BeginBP(weapon, world);
	};
	virtual void Tick(float DeltaTime, UWeaponComponent* weapon) {
		if (!bShouldTick)
			return;

		TickBP(DeltaTime, weapon);
	};

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Begin"))
	void BeginBP(UWeaponComponent* weapon, UWorld* world);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Tick"))
	void TickBP(float DeltaTime, UWeaponComponent* weapon);


public:
	void SetWorld(TObjectPtr<UWorld> world) { World = world; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UWorld* GetWorld() const override { return World; }

	void SetWeapon(TObjectPtr<UWeaponComponent> weapon) { Weapon = weapon; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UWeaponComponent* GetWeapon() { return Weapon; }

protected:
	TObjectPtr<UWorld> World;
	TObjectPtr<UWeaponComponent> Weapon;
};
