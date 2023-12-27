// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/InventoryItem.h"
#include "InventoryComponent.generated.h"

class UWeaponComponent;

UCLASS(Abstract)
class VANAGANDR_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

public:
	UPROPERTY(EditAnywhere)
	int InventoryMaxSize;

public:
	UPROPERTY(BlueprintReadOnly)
	TArray<IInventoryItem*> Inventory;	

public:
	//UFUNCTION(BlueprintCallable)
	//IInventoryItem* GetItemAtIndex(int index) {
	//	return Inventory[index];
	//}

	//UFUNCTION(BlueprintCallable)
	//void InsertItemAtIndex(IInventoryItem* item, int index) {
	//	Inventory[index] = item;
	//}
	//

	//UFUNCTION(BlueprintCallable)
	//void AddItem(IInventoryItem* item) {
	//	if (Inventory.Num() < InventoryMaxSize)
	//		Inventory.Add(item);
	//}


	//UFUNCTION(BlueprintCallable)
	//void RemoveItem(IInventoryItem* item) {
	//	if (Inventory.Contains(item))
	//		Inventory.Remove(item);
	//}
	//UFUNCTION(BlueprintCallable)
	//void RemoveItemAtIndex(IInventoryItem* item) {
	//	if (Inventory.Contains(item))
	//		Inventory.Remove(item);
	//}

	//UFUNCTION(BlueprintCallable)
	//void SwapItemIndex(int index1, int index2) {
	//	(Inventory[index1], Inventory[index2]) = (Inventory[index2], Inventory[index1]);
	//}


public:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
