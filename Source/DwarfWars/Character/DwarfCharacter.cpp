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
#include "DwarfAnimInstance.h"

// Sets default values
ADwarfCharacter::ADwarfCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	/* Camera Component Setup */
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(GetCapsuleComponent());
	PlayerCamera->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	PlayerCamera->bUsePawnControlRotation = true;

	/* Setup Local ArmsMesh (Arms that local player sees) */
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmsMesh1P"));
	Mesh1P->SetupAttachment(PlayerCamera);
	Mesh1P->bOnlyOwnerSee = true;	// Other players can't see this
	Mesh1P->bOwnerNoSee = false;	// Owner can see this
	Mesh1P->bCastDynamicShadow = false;	// Don't want it to have shadows
	Mesh1P->bReceivesDecals = false;
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Mesh1P->SetCollisionObjectType(ECC_Pawn);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);

	/* Actual mesh that other people see */
	GetMesh()->bOnlyOwnerSee = false; // Local player can't see their own mesh
	GetMesh()->bOwnerNoSee = true;	// Other players can see the actual mesh
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// GetMesh()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block); // From Epic FPS proj
	// GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block); // From Epic FPS proj
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

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

	// Attach Camera to Head "CameraSocket" 
	/*const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	PlayerCamera->AttachToComponent(Mesh1P, AttachmentRules, FName("CameraSocket"));*/

	// Attach the camera to the "Head" bone of the Mesh1P component
	//PlayerCamera->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("Head")));

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
		EnhancedInputComponent->BindAction(InputShootAction, ETriggerEvent::Triggered, this, &ADwarfCharacter::ShootInput);
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

void ADwarfCharacter::AimInput(const FInputActionValue& Value) {
	if (CombatComponent) {
		CombatComponent->SetAiming(Value.Get<bool>());
	}
}

void ADwarfCharacter::ShootInput(const FInputActionValue& Value) {
	if (CombatComponent) {
		//ScreenBoolLog("ShootInput val is: ", Value.Get<bool>());
		CombatComponent->ShootButtonPressed(Value.Get<bool>());
	}
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


void ADwarfCharacter::PlayShootMontage(bool bAiming) {
	if (CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ShootWeaponMontage) {
		AnimInstance->Montage_Play(ShootWeaponMontage);
		AnimInstance->Montage_JumpToSection(bAiming ? FName("RifleAim") : FName("RifleHip"));
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

USkeletalMeshComponent* ADwarfCharacter::GetCharacterMesh(bool bMesh1P) const {
	return bMesh1P ? Mesh1P : GetMesh();
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

void ADwarfCharacter::ScreenBoolLog(const FString& TextToLog, bool bToLog) {
	ScreenLog(TextToLog + (bToLog ? "true" : "false"));
}
