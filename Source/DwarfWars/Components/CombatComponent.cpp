// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "DwarfWars/Weapon/Weapon.h"
#include "DwarfWars/Character/DwarfCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

UCombatComponent::UCombatComponent() {
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
	bShootButtonPressed = false;
	bAiming = false;
	EquippedWeapon = nullptr;
	Character = nullptr;
}

void UCombatComponent::BeginPlay() {
	Super::BeginPlay();

	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

void UCombatComponent::SetAiming(bool bIsAiming) {
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);

	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::GetScreenCenter(FVector& Position, FVector& Direction) {
	/* Gets center of screen (and forward vector) in world space
	** Used for start position & direction of bullet spawning */
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CenterLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CenterLocation, Position, Direction);
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& HitResult) {
	
	/* TRACES FROM CENTER OF SCREEN */

	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	if (UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection)) {
		// Successfully got world coords of center of screen
		
		// Start of line trace
		FVector Start = CrosshairWorldPosition;
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
		
		if (!HitResult.bBlockingHit) {
			HitResult.ImpactPoint = End;
		} 
		else {
			// Draw Debug Sphere for hit result location
		//	DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 10, FColor::Orange, false);
		}

	}

}

void UCombatComponent::ShootButtonPressed(bool bPressed) {
	bShootButtonPressed = bPressed;

	if (bPressed) {
		
		FVector StartPos;
		FVector Direction;
		GetScreenCenter(StartPos, Direction);

		/*FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);*/
		// HitResult.ImpactPoint -> Is FVector_NetQuantize & is location of hit - can be used for tracers

		ServerShoot(StartPos, Direction);
	}
}

void UCombatComponent::ServerShoot_Implementation(const FVector_NetQuantize& StartPos, const FVector_NetQuantize& Direction) {
	// Montage is rlly jank rn, needa clean it up - just for recoil
	//if (Character && bPressed) {
	// Character->PlayShootMontage(bAiming);
	//}
	
	MulticastShoot(StartPos, Direction);

}

void UCombatComponent::MulticastShoot_Implementation(const FVector_NetQuantize& StartPos, const FVector_NetQuantize& Direction) {
	if (EquippedWeapon) EquippedWeapon->Shoot(StartPos, Direction);
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming) {
	bAiming = bIsAiming;

	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip) {
	if (Character == nullptr || WeaponToEquip == nullptr) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	// Attach to Hand Socket
	if (const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"))) {
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}

	EquippedWeapon->SetOwner(Character);
}