// Fill out your copyright notice in the Description page of Project Settings.


#include "VanagandrPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundAttenuation.h"
#include "VanagandrPlayerState.h"
#include "Components/AudioComponent.h"

#pragma region Custom Sound Functions

int32 AVanagandrPlayerCharacter::PlaySoundNetworked(FNetworkedSoundSetting SelfSettings, FNetworkedSoundSetting AlliesSettings, FNetworkedSoundSetting EnemiesSettings)
{
	int32 RandomID = FMath::Rand();
	SelfSettings.SoundID = RandomID;
	AlliesSettings.SoundID = RandomID;
	EnemiesSettings.SoundID = RandomID;

	if (IsLocallyControlled())
	{
		PlaySoundFromSetting(SelfSettings);
		SelfSettings.Sound = nullptr; //Marks this as already played
	}
	Server_PlaySoundNetworked(SelfSettings, AlliesSettings, EnemiesSettings);

	return RandomID;
}

void AVanagandrPlayerCharacter::Server_PlaySoundNetworked_Implementation(FNetworkedSoundSetting SelfSettings, FNetworkedSoundSetting AlliesSettings, FNetworkedSoundSetting EnemiesSettings)
{
	Multicast_PlaySoundNetworked(SelfSettings, AlliesSettings, EnemiesSettings);

	if (GetNetMode() == ENetMode::NM_ListenServer)
		Multicast_PlaySoundNetworked_Implementation(SelfSettings, AlliesSettings, EnemiesSettings);
}
bool AVanagandrPlayerCharacter::Server_PlaySoundNetworked_Validate(FNetworkedSoundSetting SelfSettings, FNetworkedSoundSetting AlliesSettings, FNetworkedSoundSetting EnemysSettings)
{
	return true;
}

void AVanagandrPlayerCharacter::Multicast_PlaySoundNetworked_Implementation(FNetworkedSoundSetting SelfSettings, FNetworkedSoundSetting AlliesSettings, FNetworkedSoundSetting EnemiesSettings)
{
	FNetworkedSoundSetting Setting;
	if (IsLocallyControlled())
	{
		Setting = SelfSettings;
	}
	else
	{
		//Determine for each client if we're playing the "self" sound or the "others" sound
		//We start by assuming that a non-controlled character is an enemy
		//And then compare the CurrentTeam of the local controller to this character
		//To see if we're an ally.
		Setting = EnemiesSettings;
		APlayerController* YourPC = GetGameInstance()->GetFirstLocalPlayerController();
		if (YourPC)
		{
			AVanagandrPlayerState* YourPS = YourPC->GetPlayerState<AVanagandrPlayerState>();
			AVanagandrPlayerState* ThisPS = GetPlayerState<AVanagandrPlayerState>();
			if (YourPS && ThisPS && YourPS->GetCurrentTeam() == ThisPS->GetCurrentTeam())
			{
				Setting = AlliesSettings;
			}
		}
	}

	PlaySoundFromSetting(Setting);
}

void AVanagandrPlayerCharacter::PlaySoundFromSetting(FNetworkedSoundSetting Setting)
{
	//No sound for this client
	if (!Setting.Sound) return;

	//ID already exists in map.
	if (SoundMap.Contains(Setting.SoundID)) return;

	UAudioComponent* NewSound;

	//Option 1: UI sounds
	if (Setting.bPlayWithoutLocation)
	{
		NewSound = UGameplayStatics::SpawnSound2D(this,
			Setting.Sound,
			Setting.VolumeMultiplier);
	}
	//Option 2: Attached sounds
	 else if (Setting.AttachComponent)
	{
		NewSound = UGameplayStatics::SpawnSoundAttached(
			Setting.Sound,
			Setting.AttachComponent,
			FName(),
			Setting.Location,
			EAttachLocation::KeepWorldPosition,
			false,
			Setting.VolumeMultiplier,
			1.0f,
			0.0f,
			Setting.Attenuation
		);
	}
	//Final option: Play at location
	else
	{
		NewSound = UGameplayStatics::SpawnSoundAtLocation(
			this,
			Setting.Sound,
			Setting.Location,
			FRotator::ZeroRotator,
			Setting.VolumeMultiplier,
			1.0f,
			0.0f,
			Setting.Attenuation
		);
	}

	SoundMap.Add(Setting.SoundID, NewSound);
	
}



void AVanagandrPlayerCharacter::EndSoundEarlyNetworked(int32 SoundID)
{
	EndSoundFromID(SoundID);
	Server_EndSoundEarlyNetworked(SoundID);
}

void AVanagandrPlayerCharacter::Server_EndSoundEarlyNetworked_Implementation(int32 SoundID)
{
	Multicast_EndSoundEarlyNetworked(SoundID);

	if (GetNetMode() == ENetMode::NM_ListenServer)
		Multicast_EndSoundEarlyNetworked_Implementation(SoundID);
}

bool AVanagandrPlayerCharacter::Server_EndSoundEarlyNetworked_Validate(int32 SoundID)
{
	return true;
}

void AVanagandrPlayerCharacter::Multicast_EndSoundEarlyNetworked_Implementation(int32 SoundID)
{
	EndSoundFromID(SoundID);
}

void AVanagandrPlayerCharacter::EndSoundFromID(int32 SoundID)
{
	if (SoundMap.Contains(SoundID))
	{
		SoundMap[SoundID]->Stop();
		SoundMap.Remove(SoundID);
	}
}

void AVanagandrPlayerCharacter::CleanSoundMap()
{
	TMap<int32, UAudioComponent*> SoundMapCopy = SoundMap;

	for (TPair<int32, UAudioComponent*> Pair : SoundMapCopy)
	{
		if (!IsValid(Pair.Value))
			SoundMap.Remove(Pair.Key);
	}
}

#pragma endregion

void AVanagandrPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeSinceLastSoundMapClean += DeltaTime;
	if (TimeSinceLastSoundMapClean >= SoundMapCleanFrequency)
	{
		TimeSinceLastSoundMapClean = 0.0f;
		CleanSoundMap();
	}

}
