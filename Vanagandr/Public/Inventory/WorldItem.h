// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorComponents/PickUpComponent.h"
#include "WorldItem.generated.h"

class UInventoryItemContainer;

UCLASS(Blueprintable)
class VANAGANDR_API AWorldItem : public AActor
{
	GENERATED_BODY()

public:	
	AWorldItem();

public:
	UPROPERTY(EditAnywhere)
	UPickUpComponent* PickUpComponent;
	UPROPERTY(EditAnywhere)
	UInventoryItemContainer* ItemContainer;

public:
	UInventoryItemContainer* GetItemContainer() {
		return ItemContainer;
	}
	void SetItemContainer(UInventoryItemContainer* Container) {
		if (!ItemContainer)
			this->ItemContainer = Container;
	}

public:
	void PickUpItemOverlapped(AVanagandrPlayerCharacter* PickUpChar);
	void PickUpItemManual(AVanagandrPlayerCharacter* PickUpChar);


};
