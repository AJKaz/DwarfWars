// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfAnimInstance.h"
#include "DwarfCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	bIsFruity = DwarfCharacter->IsFruity();
}
