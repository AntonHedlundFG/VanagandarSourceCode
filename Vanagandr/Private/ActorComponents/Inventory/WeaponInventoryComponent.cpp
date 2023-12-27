// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorComponents/Inventory/WeaponInventoryComponent.h"
#include "ActorComponents/WeaponComponent.h"

void UWeaponInventoryComponent::ForcePickupWeapon(AActor* weaponActor, UWeaponComponent* weapon)
{
	ForcePickupWeaponBP(weaponActor, weapon);

	ReplaceWeapon(weapon);
}

void UWeaponInventoryComponent::ReplaceWeapon(UWeaponComponent* newWeapon)
{
	switch (newWeapon->WeaponPriority)
	{
	case EWP_Primary:
		DropWeapon(PrimaryWeapon);
		PrimaryWeapon = newWeapon;
		break;
	case EWP_Secondary:
		DropWeapon(PrimaryWeapon);
		SecondaryWeapon = newWeapon;
		break;
	default:
		break;
	}
}

bool UWeaponInventoryComponent::TryPickUpWeapon(AActor* weaponActor, UWeaponComponent* newWeapon)
{
	TryPickUpWeaponBP(weaponActor, newWeapon);

	switch (newWeapon->WeaponPriority)
	{
	case EWP_Primary:
		if (!PrimaryWeapon) {
			PrimaryWeapon = newWeapon;
			return true;
		} return false;
	case EWP_Secondary:
		if (!SecondaryWeapon) {
			SecondaryWeapon = newWeapon;
			return true;
		} return false;
	default:
		return false;
	}
}

void UWeaponInventoryComponent::DropWeapon(UWeaponComponent* weapon)
{
	if (!weapon)
		return;

	switch (weapon->WeaponPriority)
	{
	case EWP_Primary:
		PrimaryWeapon = nullptr;
		break;
	case EWP_Secondary:
		SecondaryWeapon = nullptr;
		break;
	default:
		break;
	}

	weapon->Drop();

	ActiveWeapon = nullptr;
}

//A bit ugly but thats ok when we are certain that we will only have 4 possible items in inventory 
void UWeaponInventoryComponent::SwapWeaponWithIndex(int index)
{
	switch (index)
	{
	case 0:
		if (PrimaryWeapon) {
			SetActiveWeapon(PrimaryWeapon->GetOwner(), PrimaryWeapon);
		}
		break;
	case 1:
		if (SecondaryWeapon) {
			SetActiveWeapon(SecondaryWeapon->GetOwner(), SecondaryWeapon);
		}
		break;
	case 2:
		if (Knife) {
			SetActiveWeapon(Knife->GetOwner(), Knife);
		}
		break;
	case 3:
		//here we do bomb
		break;

	default:
		break;
	}
}


void UWeaponInventoryComponent::SwapWeaponWithDirection(int swapDirection)
{
}

void UWeaponInventoryComponent::EquipItemInCorrectSlot(UWeaponComponent* weaponComponent)
{
	switch (weaponComponent->WeaponPriority)
	{
	case 0:
		//ReplaceWeapon()
		break;
	default:
		break;
	}
}

TObjectPtr<UWeaponComponent> UWeaponInventoryComponent::GetWeaponInInventory(int index)
{
	return PrimaryWeapon;
}

TObjectPtr<UWeaponComponent> UWeaponInventoryComponent::GetWeaponInInventory(EWeaponPriority weaponPriority)
{
	return PrimaryWeapon;
}

TObjectPtr<UWeaponComponent> UWeaponInventoryComponent::GetHighestActivePriorityWeapon()
{
	return PrimaryWeapon;
}


void UWeaponInventoryComponent::SetActiveWeapon(AActor* weaponActor, UWeaponComponent* weapon)
{
	ActiveWeapon->Unequip();
	ActiveWeapon = weapon;
	CurrentEquippedWeapon = ActiveWeapon->WeaponPriority;
	ActiveWeapon->Equip();


	//then do animation stuff
}
