// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "VanagandrPlayerCharacter.h"
#include "WeaponComponent.generated.h"

#define PRINT(msg) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan, msg);

class UWeaponConstraint;
class UAimModifier;
class UBulletModifier;
class UBulletSpawner;
class UFirePattern;
class UWeaponEffect;
class UWeaponBehaviour;

#pragma region Enums

UENUM(BlueprintType)
enum EWeaponType : uint8
{
	EWT_Rifle UMETA(DisplayName = "Rifle"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_Sniper UMETA(DisplayName = "Sniper", ToolTip = "use a rifle you casual"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_Melee UMETA(DisplayName = "Melee")
};

UENUM(BlueprintType)
enum EWeaponPriority : uint8
{
	EWP_Primary = 0 UMETA(DisplayName = "Primary"),
	EWP_Secondary = 1 UMETA(DisplayName = "Secondary"),
	EWP_Knife = 2 UMETA(DisplayName = "Knife"),
	EWP_Nade = 3 UMETA(DisplayName = "Nade"),
	EWP_Bomb = 4 UMETA(DisplayName = "Bomb")
};

UENUM(BlueprintType)
enum EInputState : uint8
{
	EIS_Started UMETA(DisplayName = "Started"),
	EIS_Ongoing UMETA(DisplayName = "Ongoing"),
	EIS_Stopped UMETA(DisplayName = "Stopped")
};


#pragma endregion

#pragma region Structs


USTRUCT(BlueprintType, Blueprintable)
struct VANAGANDR_API FFireParams
{
	GENERATED_BODY()

	FFireParams();

public:
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Weapon Functionality")
	TObjectPtr<UFirePattern> FirePattern;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Weapon Functionality")
	TArray<TObjectPtr<UBulletModifier>> BulletModifiers;
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Weapon Functionality")
	TObjectPtr<UBulletSpawner> BulletSpawn;
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Weapon Functionality")
	TArray<TObjectPtr<UAimModifier>> AimModifiers;
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Weapon Functionality")
	TArray<TObjectPtr<UWeaponEffect>> WeaponEffects;

public:
	void ApplyAimModifiers(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter);
	FVector ApplyBulletModifiers(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter, FVector& AimDirection);
	bool CheckFirePattern(EInputState inputState);
	void PlayEffects(FBulletResults& bulletResults);

	void Fire(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter,
		FVector ShootWorldStartPoint, FVector AimDirection);
};

//Feel like this is s decent way to verify shoots on server later?
//Otherwise we'll rework this
USTRUCT(BlueprintType, Blueprintable)
struct VANAGANDR_API FBulletResults
{
	GENERATED_BODY()

	FBulletResults() {};
	FBulletResults(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter, FVector ShootWorldStartPoint, FVector AimDirection) {

		this->Controller = Controller;
		this->ShootingCharacter = ShootingCharacter;
		this->ShootWorldStartPoint = ShootWorldStartPoint;
		this->AimDirection = AimDirection;
	}

public:
	UPROPERTY(BlueprintReadWrite)
	APlayerController* Controller;

	UPROPERTY(BlueprintReadWrite)
	ACharacter* ShootingCharacter;

	UPROPERTY(BlueprintReadWrite)
	FVector ShootWorldStartPoint = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	FVector AimDirection = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	TArray<FHitResult> HitResults;

	UPROPERTY(BlueprintReadWrite)
	TMap<AActor*, float> HitDamagableActors;

};

#pragma endregion

//Unsure if we really need to declare a new delegate for each of these or if its fine to use reuse the same ones for different purposes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnFire, APlayerController*, Controller, ACharacter*, ShootingCharacter,
	FVector, ShootWorldStartPoint, FVector, AimDirection);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFireHit, FHitResult, HitResult);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInputReceived);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponPickedUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponDropped);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponEquipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponUnequipped);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFireModeToggled);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VANAGANDR_API UWeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:	
	UWeaponComponent();


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	int Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	TEnumAsByte<EWeaponType> Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	TEnumAsByte<EWeaponPriority> WeaponPriority;
	UPROPERTY(EditAnywhere, Category = "WeaponInfo")
	bool bHasSecondaryFireMode;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Weapon Functionality")
	TArray<TObjectPtr<UWeaponConstraint>> FireConstraints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Functionality")
	FFireParams PrimaryFireParams;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Functionality", meta = (EditCondition = "bHasSecondaryFireMode", EditConditionHides))
	FFireParams SecondaryFireParams;

public:
	UPROPERTY(BlueprintReadWrite)
	bool bCanToggleFireMode;
	UPROPERTY(BlueprintReadWrite)
	bool bCanFire = true;

private:
	UPROPERTY()
	TArray<TObjectPtr<UWeaponBehaviour>> AllBehaviours;

public:
	UFUNCTION(BlueprintCallable)
	void TryFire(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter,
		FVector ShootWorldStartPoint, FVector AimDirection, EInputState inputState);

	void Fire(APlayerController* Controller, AVanagandrPlayerCharacter* ShootingCharacter,
		FVector ShootWorldStartPoint, FVector AimDirection);

	UFUNCTION(BlueprintCallable)
	void ToggleSecondary() {
		if (bCanToggleFireMode) {
			bIsInSecondary = !bIsInSecondary;
			OnFireModeToggled.Broadcast();
		}
	}

	UFUNCTION(BlueprintCallable)
	void ReloadInputPressed() {
		OnReloadInput.Broadcast();
	}
	UFUNCTION(BlueprintCallable)
	void PickUp() {
		OnWeaponPickedUp.Broadcast();
	}
	UFUNCTION(BlueprintCallable)
	void Equip() {
		OnWeaponEquipped.Broadcast();
	}
	UFUNCTION(BlueprintCallable)
	void Unequip() {
		OnWeaponUnequipped.Broadcast();
	}
	UFUNCTION(BlueprintCallable)
	void Drop() {
		FDetachmentTransformRules detachmenTransformRules(EDetachmentRule::KeepWorld, false);
		DetachFromComponent(detachmenTransformRules);
		OnWeaponDropped.Broadcast();
	}


public:
	/// <summary>
	/// Incase unreal does not do it automaticly we need to remove all listeners to these delegates when a weapon is destroyed
	/// </summary>

	UPROPERTY(BlueprintAssignable, Category = "Shooting")
	FOnFire OnFire;
	UPROPERTY(BlueprintAssignable, Category = "Shooting")
	FOnFireHit OnHit;

	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnInputReceived OnReloadInput;
	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnInputReceived OnFireInputStarted;
	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnInputReceived OnFireInputStopped;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Pickup")
	FWeaponPickedUp OnWeaponPickedUp;
	UPROPERTY(BlueprintAssignable, Category = "Pickup")
	FWeaponDropped OnWeaponDropped;

	UPROPERTY(BlueprintAssignable, Category = "Equip")
	FWeaponEquipped OnWeaponEquipped;
	UPROPERTY(BlueprintAssignable, Category = "Equip")
	FWeaponUnequipped OnWeaponUnequipped;

	UPROPERTY(BlueprintAssignable, Category = "FireMode")
	FFireModeToggled OnFireModeToggled;

private:
	bool CheckConstraints();

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ACharacter* GetCharacter() { return Character; }

	UFUNCTION(BlueprintCallable)
	FFireParams GetActiveParams() {
		if (bIsInSecondary && bHasSecondaryFireMode)
			ActiveParams = SecondaryFireParams;
		else ActiveParams = PrimaryFireParams;
		return ActiveParams;
	}

private:
	ACharacter* Character;
	bool bIsActive;
	bool bIsInSecondary;
	FFireParams ActiveParams;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void BeginBehaviours();
	void TickBehaviours(float DeltaTime);
};


