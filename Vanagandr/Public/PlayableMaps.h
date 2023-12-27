// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayableMaps.generated.h"

#define TEAM_SIZE_OPTION FString("PlayerCount")
#define ROUNDS_PER_HALFTIME_OPTION FString("RoundsPerHalftime")
#define MAP_NAME FString("MapName")

USTRUCT(BlueprintType)
struct FMapLoadSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 1, ClampMax = 5))
	int TeamSize = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 3, ClampMax = 11))
	int RoundsPerHalftime = 6;

	TOptional<FString> MapName;

	FString GetOptionsStringAppendage()
	{
		FString ReturnString;
		AddOption(ReturnString, TEAM_SIZE_OPTION, FString::FromInt(FMath::Clamp(TeamSize, 1, 5)));
		AddOption(ReturnString, ROUNDS_PER_HALFTIME_OPTION, FString::FromInt(FMath::Clamp(RoundsPerHalftime, 1, 10)));
		if (MapName.IsSet())
			AddOption(ReturnString, MAP_NAME, MapName.GetValue());
		return ReturnString;
	}


private:

	void AddOption(FString& OptionList, FString OptionName, FString Value)
	{
		OptionList.Append(FString("?") + OptionName);
		if (!Value.IsEmpty())
			OptionList.Append(FString("=") + Value);
	}

};

USTRUCT(BlueprintType)
struct FMapInfo
{
	GENERATED_BODY()

public:

	/* The name shown in the Main Menu for the map
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MapName;

	/* This should be something like
	* /Game/Levels/GameLevels/L_Forge_5_Art
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MapLoadPath;

	/* An image that can be shown to represent the level in the Main Menu
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> Image;
};

/**
 * 
 */
UCLASS(Blueprintable)
class VANAGANDR_API UPlayableMaps : public UDataAsset
{
	GENERATED_BODY()
	
public:
	/* MapIndex should be the selected map's index in the Maps array
	*/
	UFUNCTION(BlueprintCallable)
	FString SelectMap(int MapIndex, FMapLoadSettings LoadSettings = FMapLoadSettings());

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMapInfo> Maps;

};
