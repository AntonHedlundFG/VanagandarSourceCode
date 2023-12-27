// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/Interactable.h"
#include "PickUpComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUp, AVanagandrPlayerCharacter*, PickUpCharacter);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VANAGANDR_API UPickUpComponent : public USphereComponent, public IInteractable
{
	GENERATED_BODY()
	
	UPickUpComponent();

public:
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnPickUp OnPickUpOverlaped;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnPickUp OnPickUpManual;

	//We let this be set manually
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bCanBePickedUp;

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Interact(AVanagandrPlayerCharacter* InteractingCharacter) override;
};
