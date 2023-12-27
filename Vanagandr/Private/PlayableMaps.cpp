// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableMaps.h"

FString UPlayableMaps::SelectMap(int MapIndex, FMapLoadSettings LoadSettings)
{
	if (MapIndex < 0 || MapIndex >= Maps.Num()) return FString();
	LoadSettings.MapName = Maps[MapIndex].MapName;
	FString LoadPath = Maps[MapIndex].MapLoadPath;
	return LoadPath
		.Append(FString("?listen?port=7779"))
		.Append(LoadSettings.GetOptionsStringAppendage());
}
