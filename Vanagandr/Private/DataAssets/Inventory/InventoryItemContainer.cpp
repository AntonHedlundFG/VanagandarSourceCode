// Fill out your copyright notice in the Description page of Project Settings.

#include "DataAssets/Inventory/InventoryItemContainer.h"
#include "Inventory/WorldItem.h"
#include "VanagandrPlayerCharacter.h"

void UInventoryItemContainer::CreateWorldItem(FTransform& spawnTransform)
{
	auto worldItem = GetWorld()->SpawnActor<AWorldItem>(WorldItem, spawnTransform);
	worldItem->SetItemContainer(this);

	OnCreateWorldItem.Broadcast();
}

void UInventoryItemContainer::CreateAndAttachInventoryItem(AVanagandrPlayerCharacter* Character)
{

}

void UInventoryItemContainer::ForcePickUpItem(AVanagandrPlayerCharacter* Character)
{
}

void UInventoryItemContainer::TryPickUpItem(AVanagandrPlayerCharacter* Character)
{
}
