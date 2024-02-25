// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Shoot(const FVector_NetQuantize& StartPos, const FVector& Direction) {
	Super::Shoot(StartPos, Direction);

	/* Only server should have authority over spawning projectiles (ie, actual shooting functionality) */
	if (HasAuthority()) {
		// Owner is the player who has the weapon
		APawn* InstigatorPawn = Cast<APawn>(GetOwner());
		UWorld* World = GetWorld();
		if (ProjectileClass && InstigatorPawn && World) {
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;
			World->SpawnActor<AProjectile>(ProjectileClass, StartPos, Direction.Rotation(), SpawnParams);
		}
	}

	/*
	* THIS COMMENTED CODE: Spawns projectile from the gun's muzzle and shoots in direction of aim
	* Not the implementation I want - keeping it here for future reference if needed
	* If using this method, need to pass in HitTarget from CombatComp.cpp instead of StartPos & Direction
	*/
	// Get Gun's Muzzle Location
	/*
	if (const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh(false)->GetSocketByName("Muzzle")) {
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh(false));

		APawn* InstigatorPawn = Cast<APawn>(GetOwner());
		UWorld* World = GetWorld();
		if (ProjectileClass && InstigatorPawn && World) {
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;

			FVector ToTarget = HitTarget - SocketTransform.GetLocation();	// Vector from Muzzle to HitTarget (aim location)
			FRotator TargetRotation = ToTarget.Rotation();

			World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
		}
	}
	*/
}
