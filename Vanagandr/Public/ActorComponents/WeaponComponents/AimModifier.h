// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActorComponents/WeaponComponents/WeaponBehaviour.h"
#include "AimModifier.generated.h"

class UWeaponComponent;
class UCurveVector;

UCLASS(Abstract, Blueprintable, EditInlineNew, CollapseCategories, Config = Weapon, defaultconfig, configdonotcheckdefaults)
class VANAGANDR_API UAimModifier : public UWeaponBehaviour
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Shooting")
	void ApplyModifier(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter);

public:
	virtual void ApplyModifier_Implementation(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter);
	
};



UCLASS(NotBlueprintable, meta = (DisplayName = "Recoil"))
class VANAGANDR_API URecoil : public UAimModifier
{
	GENERATED_BODY()


public:
	void Begin(TObjectPtr<UWeaponComponent> weapon, UWorld* world) override;
	void Tick(float DeltaTime, UWeaponComponent* weapon) override;
	
	void ApplyModifier_Implementation(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter) override;


private:
	void StartRecoil();
	void HandleRecoil(float DeltaTime);
	void EndRecoil();

	void StartRecovery();
	void HandleRecovery(float DeltaTime);
	void EndRecovery();

	void SaveCameraStartRotation(APlayerController* Controller);


public:
	UPROPERTY(EditAnywhere, Category = "Recovery")
	float fRecoverySpeed;
	UPROPERTY(EditAnywhere, Category = "Recovery")
	float fRecoveryMaxTime;


	UPROPERTY(EditAnywhere, Category = "Recoil")
	float fEndRecoilDelay;
	UPROPERTY(EditAnywhere, Category = "Recoil")
	float fRecoilTotalBumpPerShot;

	UPROPERTY(EditAnywhere, Category = "Recoil")
	float fMaxRecoilPoint;
	UPROPERTY(EditAnywhere, Category = "Recoil")
	float fHitMaxRecoilResetPointOnCurve;

	UPROPERTY(EditAnywhere, Category = "Recoil")
	float fRecoilSpeed;

	UPROPERTY(EditAnywhere, Category = "Recoil")
	UCurveVector* RecoilCurve;

private:
	float TimeSinceLastFire;

	float fRecoilFloat;
	float fRecoilLerpT;

	float fRecoilStartPoint;
	float fRecoilTarget;

	float fRecoveryStartPoint;
	float fRecoveryFloat;

	bool bInRecoil;
	bool bInRecovery;

	FRotator CameraStartRotation;
	FRotator CameraCurrentRotation;

	FRotator CameraStartRecoveryRotation;

	FRotator RecoilDeltaRotation;
	FRotator PlayerDeltaRotation;

	FRotator Del;

private:
	APlayerController* CurrentController;

};
