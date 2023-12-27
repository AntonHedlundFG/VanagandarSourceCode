// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponents/Inventory/InventoryComponent.h"
#include "ActorComponents/WeaponComponent.h"
#include "WeaponInventoryComponent.generated.h"

class UWeaponComponent;

/// <summary>
/// This is an ugly nest of switch-statements atm
/// We WILL fix this in time, but now is not that time
/// </summary>

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VANAGANDR_API UWeaponInventoryComponent : public UInventoryComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(Blueprintcallable)
	bool TryPickUpWeapon(AActor* weaponActor, UWeaponComponent* newWeapon);
	UFUNCTION(Blueprintcallable)
	void ForcePickupWeapon(AActor* weaponActor, UWeaponComponent* weapon);


	UFUNCTION(Blueprintcallable)
	void  ReplaceWeapon(UWeaponComponent* newWeapon);


	UFUNCTION(Blueprintcallable)
	void DropWeapon(UWeaponComponent* weapon);

	UFUNCTION(Blueprintcallable)
	void SwapWeaponWithIndex(int index);

	//SwapDirection set to less than 0 will go down in priority
	//Else it goes up
	UFUNCTION(Blueprintcallable)
	void SwapWeaponWithDirection(int swapDirection);

	UFUNCTION(Blueprintcallable)
	void EquipItemInCorrectSlot(UWeaponComponent* weaponComponent);

private:
	TObjectPtr<UWeaponComponent> GetWeaponInInventory(int weaponIndex);
	TObjectPtr<UWeaponComponent> GetWeaponInInventory(EWeaponPriority weaponPriority);

	TObjectPtr<UWeaponComponent> GetHighestActivePriorityWeapon();

public:
	void SetActiveWeapon(AActor* weaponActor, UWeaponComponent* weapon);
	UWeaponComponent* GetActiveWeapon() {
		return ActiveWeapon;
	}

public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "TryPickUpWeapon"))
	void TryPickUpWeaponBP(AActor* weaponActor, UWeaponComponent* weapon);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ForcePickUpWeapon"))
	void ForcePickupWeaponBP(AActor* weaponActor, UWeaponComponent* weapon);

public:
	UPROPERTY(BlueprintReadWrite, Category = "Weapons")
	TObjectPtr<UWeaponComponent> ActiveWeapon;

	UPROPERTY(VisibleAnywhere);
	TEnumAsByte<EWeaponPriority> CurrentEquippedWeapon;

	//We are hard coding since we will 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TObjectPtr<UWeaponComponent> PrimaryWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TObjectPtr<UWeaponComponent> SecondaryWeapon;

	//We should probly want this here since it where we handle weapon swaping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TObjectPtr<UWeaponComponent> Knife;

	//TStaticArray<TObjectPtr<UWeaponComponent>, > Inventory


	//add this when we have it
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TObjectPtr<Bomb> Bomb;*/
	
};
