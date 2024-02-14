// Fill out your copyright notice in the Description page of Project Settings.

#include "DwarfCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine.h"
#include "Components/AudioComponent.h"


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

	/* Animation Stuff */
	bIsFruity = false;

	/* Attack Collision Stuff */
	RightHandCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandCollisionBox"));
	RightHandCollisionBox->SetupAttachment(RootComponent);
	RightHandCollisionBox->SetCollisionProfileName("NoCollision");
	RightHandCollisionBox->SetNotifyRigidBodyCollision(false);
	RightHandCollisionBox->SetGenerateOverlapEvents(false);

	/* Audio */
	PunchAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PunchAudioComponent"));
	PunchAudioComponent->SetupAttachment(RootComponent);

	/* Misc */
	bIsAttacking = false;
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
	RightHandCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, FName("RightHandSocket"));
	RightHandCollisionBox->OnComponentHit.AddDynamic(this, &ADwarfCharacter::OnPunchHit);

	PlayerCamera->AttachToComponent(GetMesh(), AttachmentRules, FName("CameraSocket"));

	/* Audio */
	if (PunchAudioComponent && PunchSoundCue) PunchAudioComponent->SetSound(PunchSoundCue);

}

void ADwarfCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADwarfCharacter, bIsFruity);
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
		EnhancedInputComponent->BindAction(InputPunchAction, ETriggerEvent::Triggered, this, &ADwarfCharacter::Punch);
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

void ADwarfCharacter::Pause() {
	// TODO: Open pause menu - "Pause" may not be best word for this
	// as it won't actually pause the game
	UE_LOG(LogTemp, Warning, TEXT("Pause Pressed"));
}

void ADwarfCharacter::PunchAttackStart() {
	RightHandCollisionBox->SetCollisionProfileName("Weapon");
	RightHandCollisionBox->SetNotifyRigidBodyCollision(true);
}

void ADwarfCharacter::PunchAttackEnd() {
	RightHandCollisionBox->SetCollisionProfileName("NoCollision");
	RightHandCollisionBox->SetNotifyRigidBodyCollision(false);
	bIsAttacking = false;
}

void ADwarfCharacter::Punch() {
	if (bIsAttacking) return;

	bIsAttacking = true;

	// Play punch montage locally so you don't feel any lag
	if (!HasAuthority()) LocalPunch();
	ServerPunch();
}

void ADwarfCharacter::ServerPunch_Implementation() {
	MulticastPunch();
}

bool ADwarfCharacter::ServerPunch_Validate() {
	return true;
}

void ADwarfCharacter::MulticastPunch_Implementation() {
	// If server or client that's not THIS client, return
	if (IsLocallyControlled() && !HasAuthority()) return;
	LocalPunch();
}

void ADwarfCharacter::LocalPunch() {
	if (PunchAttackMontage) PlayAnimMontage(PunchAttackMontage, 1.f, FName("Punch_Start_1"));
}

void ADwarfCharacter::OnPunchHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, __FUNCTION__);

	if (PunchAudioComponent && !PunchAudioComponent->IsPlaying()) {
		PunchAudioComponent->SetPitchMultiplier(FMath::RandRange(0.9f, 1.4f));
		PunchAudioComponent->Play(0.f);
	}
}

void ADwarfCharacter::SetIsFruity(bool bFruity) {
	bIsFruity = bFruity;
}

bool ADwarfCharacter::IsFruity() {
	return bIsFruity;
}

bool ADwarfCharacter::IsMoving() {
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement()) {
		return MovementComponent->GetCurrentAcceleration().Size() > 0.f;
	}
	return false;
}
