// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfAnimInstance.h"
#include "DwarfCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DwarfWars/Weapon/Weapon.h"

void UDwarfAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();
	DwarfCharacter = Cast<ADwarfCharacter>(TryGetPawnOwner());
}

void UDwarfAnimInstance::NativeUpdateAnimation(float DeltaTime) {
	Super::NativeUpdateAnimation(DeltaTime);

	if (DwarfCharacter == nullptr) DwarfCharacter = Cast<ADwarfCharacter>(TryGetPawnOwner());
	if (DwarfCharacter == nullptr) return;
	
	FVector Velocity = DwarfCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsAccelerating = DwarfCharacter->IsMoving();
	bIsInAir = DwarfCharacter->GetCharacterMovement()->IsFalling();
	bWeaponEquipped = DwarfCharacter->IsWeaponEquipped();
	bAiming = DwarfCharacter->IsAiming();

	/* For Running Blendspace */
	// Offset Yaw while strafing
	FRotator AimRotation = DwarfCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(DwarfCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 5.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = DwarfCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	/* For AimOffsets */
	AO_Pitch = DwarfCharacter->GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !DwarfCharacter->IsLocallyControlled()) {
		// Map Pitch from [270, 360) to [-90, 0)
		// Why: Because UE compresses FRotators to unsigned short, so value -90 == 270 and 0 == 360
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}

	/* Setup Left Hand FABRIK */
	EquippedWeapon = DwarfCharacter->GetEquippedWeapon();
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh(false) && DwarfCharacter->GetMesh()) {
		LeftHandTransform = EquippedWeapon->GetWeaponMesh(false)->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		
		// Want "bone" space for weapon, not world space
		FVector OutPosition;
		FRotator OutRotation;
		DwarfCharacter->GetMesh()->TransformToBoneSpace(FName("RightHand"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);

		// Set Location & Rotation for Left Hand Transform now
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));
	}
}
