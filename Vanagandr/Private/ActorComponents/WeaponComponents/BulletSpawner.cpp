// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/WeaponComponents/BulletSpawner.h"

FBulletResults UBulletSpawner::CreateBullet_Implementation(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter, FVector ShootWorldStartPoint, FVector AimDirection)
{
	return FBulletResults(Controller, ShootingCharacter, ShootWorldStartPoint, AimDirection);
}
