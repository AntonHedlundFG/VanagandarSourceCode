// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PBPlayerCharacter.h"
#include "VanagandrPlayerCharacter.generated.h"

class USoundAttenuation;
class USoundBase;
class UAudioComponent;

USTRUCT(BlueprintType)
struct FNetworkedSoundSetting
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Custom Sound Functions")
	TObjectPtr<USoundBase> Sound = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Custom Sound Functions")
	bool bPlayWithoutLocation = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Custom Sound Functions")
	FVector Location = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Custom Sound Functions")
	FRotator Rotation = FRotator::ZeroRotator;

	/** Optional attach component. If left empty, will just play at Location.
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Custom Sound Functions")
	TObjectPtr<USceneComponent> AttachComponent = nullptr;

	/** Optional attenuation settings. If left empty, will not override attenuation.
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Custom Sound Functions")
	TObjectPtr<USoundAttenuation> Attenuation = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Custom Sound Functions")
	float VolumeMultiplier = 1.0f;

	UPROPERTY()
	int32 SoundID = 0;

};

/**
 * 
 */
UCLASS()
class VANAGANDR_API AVanagandrPlayerCharacter : public APBPlayerCharacter
{
	GENERATED_BODY()
	
#pragma region Custom Sound Functions

public:

	/** This function can be called on a client or server. It will play sounds for everyone
	* but you can choose to play sounds differently on the calling client than everyone else.
	* The returned integer is an ID for the sound, which can be used to end it early.
	*/
	UFUNCTION(BlueprintCallable, Category = "Custom Sound Functions")
	int32 PlaySoundNetworked(FNetworkedSoundSetting SelfSettings, FNetworkedSoundSetting AlliesSettings, FNetworkedSoundSetting EnemiesSettings);

	/** Ends a sound early using its semi-unique ID, if it exists.
	* It will happen instantly locally, and then replicate to all other clients.
	*/
	UFUNCTION(BlueprintCallable, Category = "Custom Sound Functions")
	void EndSoundEarlyNetworked(int32 SoundID);
	
private:

	/** Do not call this manually, it is used as an intermediate for Server_PlaySoundNetworked
	*/
	UFUNCTION(Server, Reliable, WithValidation, Category = "Custom Sound Functions")
	void Server_PlaySoundNetworked(FNetworkedSoundSetting SelfSettings, FNetworkedSoundSetting AlliesSettings, FNetworkedSoundSetting EnemiesSettings);

	/** Do not call this manually, it is used as an intermediate for Server_PlaySoundNetworked
	*/
	UFUNCTION(NetMulticast, Reliable, Category = "Custom Sound Functions")
	void Multicast_PlaySoundNetworked(FNetworkedSoundSetting SelfSettings, FNetworkedSoundSetting AlliesSettings, FNetworkedSoundSetting EnemiesSettings);

	UFUNCTION()
	void PlaySoundFromSetting(FNetworkedSoundSetting Setting);

	UFUNCTION(Server, Reliable, WithValidation, Category = "Custom Sound Functions")
	void Server_EndSoundEarlyNetworked(int32 SoundID);

	UFUNCTION(NetMulticast, Reliable, Category = "Custom Sound Functions")
	void Multicast_EndSoundEarlyNetworked(int32 SoundID);

	UFUNCTION()
	void EndSoundFromID(int32 SoundID);

	//Here we store each spawned sound in a map with its randomized ID as a key
	//So that we can easily find sounds that should be cancelled early.
	UPROPERTY()
	TMap<int32, UAudioComponent*> SoundMap;

	UFUNCTION()
	void CleanSoundMap();

	UPROPERTY()
	float SoundMapCleanFrequency = 10.0f;

	UPROPERTY()
	float TimeSinceLastSoundMapClean = 0.0f;

#pragma endregion

protected:
	void Tick(float DeltaTime) override;


};
