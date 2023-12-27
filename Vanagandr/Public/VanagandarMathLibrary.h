// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VanagandarMathLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VANAGANDR_API UVanagandarMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	static float InverseLerp(float a, float b, float value);
	
};
