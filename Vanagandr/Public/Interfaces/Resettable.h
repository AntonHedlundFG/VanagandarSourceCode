// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Resettable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UResettable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VANAGANDR_API IResettable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//IMPORTANT: Will only be called on the server, replicate accordingly
	UFUNCTION(BlueprintImplementableEvent)
	void VanagandrReset();

};
