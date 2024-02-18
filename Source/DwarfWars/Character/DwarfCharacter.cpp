// Fill out your copyright notice in the Description page of Project Settings.

#include "DwarfCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine.h"
#include "DwarfWars/Components/CombatComponent.h"
#include "DwarfWars/Weapon/Weapon.h"

// Sets default values
ADwarfCharacter::ADwarfCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	/* Camera Component Setup */
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(GetMesh());
	PlayerCamera->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	PlayerCamera->bUsePawnControlRotation = true;

	/* Player Settings Setup */
	MouseSensitivity = 0.5f;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}

void ADwarfCharacter::BeginPlay() {
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	// Attach Collision Components to Proper Sockets
	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	PlayerCamera->AttachToComponent(GetMesh(), AttachmentRules, FName("CameraSocket"));
}

void ADwarfCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ADwarfCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ADwarfCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();

	if (CombatComponent) {
		CombatComponent->Character = this;
	}
}

void ADwarfCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ADwarfCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(InputMoveAction, ETriggerEvent::Triggered, this, &ADwarfCharacter::Move);
		EnhancedInputComponent->BindAction(InputLookAction, ETriggerEvent::Triggered, this, &ADwarfCharacter::Look);
		EnhancedInputComponent->BindAction(InputPauseAction, ETriggerEvent::Triggered, this, &ADwarfCharacter::Pause);
		EnhancedInputComponent->BindAction(InputJumpAction, ETriggerEvent::Triggered, this, &ADwarfCharacter::Jump);
		EnhancedInputComponent->BindAction(InputEquipAction, ETriggerEvent::Triggered, this, &ADwarfCharacter::Equip);
		EnhancedInputComponent->BindAction(InputAimAction, ETriggerEvent::Triggered, this, &ADwarfCharacter::AimInput);
	}
}

void ADwarfCharacter::Move(const FInputActionValue& Value) {
	const FVector2D MoveVector = Value.Get<FVector2D>();
	AddMovementInput(GetActorRightVector(), MoveVector.X);
	AddMovementInput(GetActorForwardVector(), MoveVector.Y);
}

void ADwarfCharacter::Look(const FInputActionValue& Value) {
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisValue.X * MouseSensitivity);
	AddControllerPitchInput(LookAxisValue.Y * MouseSensitivity);
}

void ADwarfCharacter::Jump() {
	Super::Jump();
}

void ADwarfCharacter::Pause() {
	// TODO: Open pause menu - "Pause" may not be best word for this
	// as it won't actually pause the game
	DebugLog("Pause Pressed");
}

void ADwarfCharacter::Equip() {
	if (CombatComponent) {
		if (HasAuthority()) {
			CombatComponent->EquipWeapon(OverlappingWeapon);
		}
		else {
			ServerEquipPressed();
		}
	}
}

void ADwarfCharacter::AimInput(const FInputActionValue& Value) {
	if (CombatComponent) {
		CombatComponent->SetAiming(Value.Get<bool>());
	}
}

void ADwarfCharacter::ServerEquipPressed_Implementation() {
	if (CombatComponent) {
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}
}

void ADwarfCharacter::SetOverlappingWeapon(AWeapon* Weapon) {
	if (OverlappingWeapon) {
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;
	if (IsLocallyControlled() && OverlappingWeapon) {
		OverlappingWeapon->ShowPickupWidget(true);
	}
}

void ADwarfCharacter::OnRep_OverlappingWeapon(AWeapon* PrevWeapon) {
	if (OverlappingWeapon) {
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (PrevWeapon) {
		PrevWeapon->ShowPickupWidget(false);
	}
}

bool ADwarfCharacter::IsWeaponEquipped() {
	return (CombatComponent && CombatComponent->EquippedWeapon);
}

bool ADwarfCharacter::IsAiming() {
	return (CombatComponent && CombatComponent->bAiming);
}

AWeapon* ADwarfCharacter::GetEquippedWeapon() {
	if (CombatComponent == nullptr) return nullptr;
	return CombatComponent->EquippedWeapon;
}

bool ADwarfCharacter::IsMoving() {
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement()) {
		return MovementComponent->GetCurrentAcceleration().Size() > 0.f;
	}
	return false;
}

void ADwarfCharacter::ScreenLog(const FString& TextToLog) {
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TextToLog);
}

void ADwarfCharacter::DebugLog(const FString& TextToLog) {
	UE_LOG(LogTemp, Warning, TEXT("%s"), *TextToLog);
}