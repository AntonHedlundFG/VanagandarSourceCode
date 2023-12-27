// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/PickUpComponent.h"
#include "Net/UnrealNetwork.h"
#include "VanagandrPlayerCharacter.h"

UPickUpComponent::UPickUpComponent() {

	UPickUpComponent::SetIsReplicatedByDefault(true);
}


void UPickUpComponent::BeginPlay()
{

	OnComponentBeginOverlap.AddDynamic(this, &UPickUpComponent::OnSphereBeginOverlap);
}

void UPickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bCanBePickedUp)
		return;

	AVanagandrPlayerCharacter* Character = Cast<AVanagandrPlayerCharacter>(OtherActor);
	if (Character != nullptr)
	{
		OnPickUpOverlaped.Broadcast(Character);
	}
}

void UPickUpComponent::Interact(AVanagandrPlayerCharacter* InteractingCharacter)
{
	if (!bCanBePickedUp)
		return;

	if (InteractingCharacter != nullptr)
	{
		OnPickUpManual.Broadcast(InteractingCharacter);
	}
}


void UPickUpComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPickUpComponent, bCanBePickedUp);
}