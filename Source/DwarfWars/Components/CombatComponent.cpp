// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "DwarfWars/Weapon/Weapon.h"
#include "DwarfWars/Character/DwarfCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "DwarfWars/Character/DwarfPlayerController.h"
#include "DwarfWars/HUD/DwarfHUD.h"

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

	SetHUDCrosshairs(DeltaTime);
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

void UCombatComponent::SetHUDCrosshairs(float DeltaTime) {
	if (Character == nullptr || Character->Controller == nullptr) return;

	Controller = Controller == nullptr ? Cast<ADwarfPlayerController>(Character->Controller) : Controller;
	if (Controller == nullptr) return;

	HUD = HUD == nullptr ? Cast<ADwarfHUD>(Controller->GetHUD()) : HUD;
	if (HUD == nullptr) return;

	FHUDPackage HUDPackage;
	if (EquippedWeapon) {
		HUDPackage.CrosshairCenter = EquippedWeapon->CrosshairCenter;
		HUDPackage.CrosshairTop = EquippedWeapon->CrosshairTop;
		HUDPackage.CrosshairBottom = EquippedWeapon->CrosshairBottom;
		HUDPackage.CrosshairLeft = EquippedWeapon->CrosshairLeft;
		HUDPackage.CrosshairRight = EquippedWeapon->CrosshairRight;

		// Calculate Crosshair Spread
		FVector2D MoveSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
		FVector2D VelocityMultiplierRange(0.f, 1.f);
		FVector Velocity = Character->GetVelocity();
		Velocity.Z = 0.f;
	 	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(MoveSpeedRange, VelocityMultiplierRange, Velocity.Size());

		if (Character->GetCharacterMovement()->IsFalling()) {
			CrosshairAirFactor = FMath::FInterpTo(CrosshairAirFactor, 2.25f, DeltaTime, 2.25f);
		}
		else {
			CrosshairAirFactor = FMath::FInterpTo(CrosshairAirFactor, 0.f, DeltaTime, 30.f);
		}

		HUDPackage.CrosshairSpread = CrosshairVelocityFactor + CrosshairAirFactor;
	}
	else {
		// If no weapon is equipped, don't draw crosshairs
		HUDPackage.CrosshairCenter = nullptr;
		HUDPackage.CrosshairTop = nullptr;
		HUDPackage.CrosshairBottom = nullptr;
		HUDPackage.CrosshairLeft = nullptr;
		HUDPackage.CrosshairRight = nullptr;
		HUDPackage.CrosshairSpread = 0.f;
	}

	HUD->SetHUDPackage(HUDPackage);

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
		// HitResult.ImpactPoint -> Is FVector_NetQuantize & is location of hit - can be used for tracers
		/*FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);*/
		// SimulateShoot(HitResult.ImpactPoint);

		FVector StartPos;
		FVector Direction;
		GetScreenCenter(StartPos, Direction);

		/*if (Character->HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("SERVER Direction Vector 2: %s"), *Direction.ToString());
		if (!Character->HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("CLIENT Direction Vector 2: %s"), *Direction.ToString());*/

		ServerShoot(StartPos, Direction);
	}
}

void UCombatComponent::ServerShoot_Implementation(const FVector_NetQuantize& StartPos, const FVector& Direction) {
	// Montage is rlly jank rn, needa clean it up - just for recoil
	//if (Character && bPressed) {
	// Character->PlayShootMontage(bAiming);
	//}
	
	/*if (Character->HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("SERVER Direction Vector in server shoot: %s"), *Direction.ToString());
	if (!Character->HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("CLIENT Direction Vector in server shoot: %s"), *Direction.ToString());*/

	MulticastShoot(StartPos, Direction);

}

void UCombatComponent::MulticastShoot_Implementation(const FVector_NetQuantize& StartPos, const FVector& Direction) {
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
	if (const USkeletalMeshSocket* HandSocket = Character->GetCharacterMesh(false)->GetSocketByName(FName("RightHandSocket"))) {
		HandSocket->AttachActor(EquippedWeapon, Character->GetCharacterMesh(false));
	}
	
	EquippedWeapon->SetOwner(Character);

	// Attach 1st person weapon mesh to 1st person char mesh
	if (const USkeletalMeshSocket* HandSocket = Character->GetCharacterMesh(true)->GetSocketByName(FName("RightHandSocket_1P"))) {
		if (USkeletalMeshComponent* WeaponMesh = EquippedWeapon->GetWeaponMesh(true)) {
			WeaponMesh->AttachToComponent(Character->GetCharacterMesh(true), FAttachmentTransformRules::SnapToTargetIncludingScale, HandSocket->SocketName);
			WeaponMesh->bHiddenInGame = false;
		}
	}
	// Attach 3rd person weapon mesh to 3rd person char mesh
	if (const USkeletalMeshSocket* HandSocket = Character->GetCharacterMesh(false)->GetSocketByName(FName("RightHandSocket"))) {
		if (USkeletalMeshComponent* WeaponMesh = EquippedWeapon->GetWeaponMesh(false)) {
			WeaponMesh->AttachToComponent(Character->GetCharacterMesh(false), FAttachmentTransformRules::SnapToTargetIncludingScale, HandSocket->SocketName);
		}
	}
}