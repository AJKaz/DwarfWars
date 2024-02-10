// Fill out your copyright notice in the Description page of Project Settings.

#include "DwarfCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/WidgetComponent.h"

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

	// Show Overhead Widget For Player Name
	/*OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);*/
}

void ADwarfCharacter::BeginPlay() {
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

void ADwarfCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(InputMoveAction, ETriggerEvent::Triggered, this, &ADwarfCharacter::Move);
		//EnhancedInputComponent->BindAction(InputMoveAction, ETriggerEvent::Completed, this, &ADwarfCharacter::StopMoving);
		EnhancedInputComponent->BindAction(InputLookAction, ETriggerEvent::Triggered, this, &ADwarfCharacter::Look);
		EnhancedInputComponent->BindAction(InputJumpAction, ETriggerEvent::Triggered, this, &ADwarfCharacter::Jump);
		EnhancedInputComponent->BindAction(InputPauseAction, ETriggerEvent::Triggered, this, &ADwarfCharacter::Pause);
	}
}

void ADwarfCharacter::Move(const FInputActionValue& Value) {
	const FVector2D MoveVector = Value.Get<FVector2D>();
	AddMovementInput(GetActorRightVector(), MoveVector.X);
	AddMovementInput(GetActorForwardVector(), MoveVector.Y);
}

//void ADwarfCharacter::StopMoving() {
//}

void ADwarfCharacter::Look(const FInputActionValue& Value) {
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisValue.X * MouseSensitivity);
	AddControllerPitchInput(LookAxisValue.Y * MouseSensitivity);
}

void ADwarfCharacter::Jump() {
	//Super::Jump();
}

void ADwarfCharacter::Landed(const FHitResult& Hit) {
	Super::Landed(Hit);
}

void ADwarfCharacter::Pause() {
	// TODO: Open pause menu - "Pause" may not be best word for this
	// as it won't actually pause the game
}

void ADwarfCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}