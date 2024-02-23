// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

class AProjectile;

/**
 * 
 */
UCLASS()
class DWARFWARS_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Shoot(const FVector_NetQuantize& StartPos, const FVector& Direction) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileClass;

};
