// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedCalculator.h"

// Sets default values
ASpeedCalculator::ASpeedCalculator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpeedCalculator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpeedCalculator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Interval <= 0) {
		Interval = 30;
		if(Target != nullptr) {
			
			float XVel = Target->GetVelocity().X;
			float YVel = Target->GetVelocity().Y;

			FVector Speed = FVector(XVel,YVel,0);
			
			UE_LOG(LogTemp, Log, TEXT("Speed: %f"), Speed.Size());
		}
	}

	if(Interval > 0) {
		Interval -= 1;
	}
}

