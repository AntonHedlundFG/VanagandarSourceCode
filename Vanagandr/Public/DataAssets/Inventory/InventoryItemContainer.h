// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InventoryItemContainer.generated.h"

class AWorldItem;
class IInventoryItem;
class AVanagandrPlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCreateWorldItem);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FForcePickUpItem, AVanagandrPlayerCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTryPickUpItem, AVanagandrPlayerCharacter*, Character);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttachInventoryItem, AVanagandrPlayerCharacter*, Character);


UCLASS(BlueprintType)
class VANAGANDR_API UInventoryItemContainer : public UDataAsset
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AWorldItem> WorldItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> InventoryItem;

public:
	UFUNCTION(BlueprintCallable)
	void CreateWorldItem(FTransform& spawnTransform);

	UFUNCTION(BlueprintCallable)
	void CreateAndAttachInventoryItem(AVanagandrPlayerCharacter* Character);

	UFUNCTION(BlueprintCallable)
	void ForcePickUpItem(AVanagandrPlayerCharacter* Character);
	UFUNCTION(BlueprintCallable)
	void TryPickUpItem(AVanagandrPlayerCharacter* Character);

public:
	UPROPERTY(BlueprintAssignable)
	FCreateWorldItem OnCreateWorldItem;
	UPROPERTY(BlueprintAssignable)
	FForcePickUpItem OnForcePickUpItem;
	UPROPERTY(BlueprintAssignable)
	FAttachInventoryItem OnAttachInventoryItem;
};
