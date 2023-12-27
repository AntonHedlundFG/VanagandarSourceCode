// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorComponents/WeaponComponents/AimModifier.h"
#include "Curves/CurveVector.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

void UAimModifier::ApplyModifier_Implementation(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter)
{
}

void URecoil::Begin(TObjectPtr<UWeaponComponent> weapon, UWorld* world)
{
	Super::Begin(weapon, world);

}

void URecoil::Tick(float DeltaTime, UWeaponComponent* weapon)
{
	Super::Tick(DeltaTime, weapon);

	if (!CurrentController)
		return;

	TimeSinceLastFire += DeltaTime;
	if (TimeSinceLastFire > fEndRecoilDelay && bInRecoil) {
		EndRecoil();
	}

	if (bInRecovery)
		HandleRecovery(DeltaTime);

	if (bInRecoil)
		HandleRecoil(DeltaTime);
}

void URecoil::ApplyModifier_Implementation(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter)
{
	CurrentController = Controller;

	if (!bInRecoil || !bInRecovery)
		SaveCameraStartRotation(Controller);

	fRecoilStartPoint = fRecoilFloat;
	fRecoilLerpT = 0;

	fRecoilTarget += fRecoilTotalBumpPerShot;

	TimeSinceLastFire = 0;

	if (!bInRecoil)
		StartRecoil();
}

/// <summary>
/// Recoil
/// </summary>

void URecoil::StartRecoil()
{
	bInRecoil = true;
	PlayerDeltaRotation = FRotator(0.0f, 0.0f, 0.0f);
	RecoilDeltaRotation = FRotator(0.0f, 0.0f, 0.0f);

	EndRecovery();
}

void URecoil::HandleRecoil(float DeltaTime)
{
	CameraCurrentRotation = CurrentController->GetControlRotation();

	//Lerps our recoile tracking point to avoid "Hicky" recoil
	fRecoilLerpT += DeltaTime * fRecoilSpeed;
	fRecoilLerpT = FMath::Clamp(fRecoilLerpT, 0, 1);
	fRecoilFloat = FMath::Lerp(fRecoilStartPoint, fRecoilTarget, fRecoilLerpT);

	//Gets the rotation made by the player
	//Compares Current to the start camera rotation and subtracts rotation made by recoil
	PlayerDeltaRotation = CameraCurrentRotation - CameraStartRotation - RecoilDeltaRotation;

	//Gets the point in our vector curve where we want to be with our recoil
	FVector RecoilVec = RecoilCurve->GetVectorValue(fRecoilFloat);

	//Updates our tracking of how much rotation is made by recoil
	RecoilDeltaRotation.Roll = 0;
	RecoilDeltaRotation.Pitch = (RecoilVec.Y);
	RecoilDeltaRotation.Yaw = (RecoilVec.Z);

	//Sets rotation to camera by adding the player made rotation and recoil made rotation to the start rotation
	CurrentController->SetControlRotation(CameraStartRotation + PlayerDeltaRotation + RecoilDeltaRotation);
}

void URecoil::EndRecoil()
{
	bInRecoil = false;
	StartRecovery();
}



/// <summary>
/// Recovery
/// </summary>

void URecoil::StartRecovery()
{
	bInRecovery = true;
	fRecoveryFloat = FMath::Clamp(fRecoilFloat, 0, fRecoveryMaxTime);
	fRecoilFloat = 0;
	fRecoveryStartPoint = fRecoveryFloat;
	CameraStartRecoveryRotation = CurrentController->GetControlRotation();
}

void URecoil::HandleRecovery(float DeltaTime)
{
	//Decrement recoil timers
	fRecoveryFloat = fRecoveryFloat > 0 ? fRecoveryFloat - DeltaTime * fRecoverySpeed : 0;
	fRecoilTarget = fRecoveryFloat;

	//Check if we are back in our start rotation 
	//Note: might want to add playerdelta to camera start pitch
	if (fRecoveryFloat >= 0)
	{
		CameraCurrentRotation = CurrentController->GetControlRotation();

		auto T = fRecoveryFloat / fRecoveryStartPoint;
		auto targetRot = FMath::Lerp(CameraCurrentRotation - RecoilDeltaRotation, CameraStartRecoveryRotation, T);

		CurrentController->SetControlRotation(targetRot);
		RecoilDeltaRotation = RecoilDeltaRotation + (CurrentController->GetControlRotation() - CameraCurrentRotation);

		if (fRecoveryFloat <= 0)
			EndRecovery();
	}
}

void URecoil::EndRecovery()
{
	bInRecovery = false;
}


/// Helpers
void URecoil::SaveCameraStartRotation(APlayerController* Controller)
{
	if (Controller)
		CameraStartRotation = Controller->GetControlRotation();
}
