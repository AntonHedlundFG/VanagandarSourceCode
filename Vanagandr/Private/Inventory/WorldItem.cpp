// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/WorldItem.h"
#include "DataAssets/Inventory/InventoryItemContainer.h"

AWorldItem::AWorldItem()
{
	PrimaryActorTick.bCanEverTick = false;
	PickUpComponent = CreateDefaultSubobject<UPickUpComponent>(TEXT("PickUpComponent"));
	
	//PickUpComponent->OnPickUpManual.AddDynamic(this, &AWorldItem::PickUpItemManual);
	//PickUpComponent->OnPickUpOverlaped.AddDynamic(this, &AWorldItem::PickUpItemOverlapped);
}

void AWorldItem::PickUpItemOverlapped(AVanagandrPlayerCharacter* PickUpChar)
{
	if (ItemContainer)
		ItemContainer->TryPickUpItem(PickUpChar);
}

void AWorldItem::PickUpItemManual(AVanagandrPlayerCharacter* PickUpChar)
{
	if (ItemContainer)
		ItemContainer->ForcePickUpItem(PickUpChar);
}
