// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;
class ADwarfCharacter;
class ADwarfPlayerController;
class ADwarfHUD;

#define TRACE_LENGTH 80000.f

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DWARFWARS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Gives DwarfCharacter full access to CombatComponent (even private variables) */
	friend class ADwarfCharacter;

	void EquipWeapon(AWeapon* WeaponToEquip);

protected:
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	void ShootButtonPressed(bool bPressed);

	UFUNCTION(Server, Reliable)
	void ServerShoot(const FVector_NetQuantize& StartPos, const FVector_NetQuantize& Direction);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShoot(const FVector_NetQuantize& StartPos, const FVector_NetQuantize& Direction);

	void TraceUnderCrosshairs(FHitResult& HitResult);

	void SetHUDCrosshairs(float DeltaTime);

private:
	UPROPERTY()
	ADwarfCharacter* Character;

	UPROPERTY()
	ADwarfPlayerController* Controller;

	UPROPERTY()
	ADwarfHUD* HUD;

	UPROPERTY(Replicated)
	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere, Category = Movement)
	float BaseWalkSpeed;
	
	UPROPERTY(EditAnywhere, Category = Movement)
	float AimWalkSpeed;

	bool bShootButtonPressed;
		
	void GetScreenCenter(FVector& Position, FVector& Direction);

	float CrosshairVelocityFactor;
	float CrosshairAirFactor;
};
