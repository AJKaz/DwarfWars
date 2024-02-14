// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "DwarfCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class UWidgetComponent;
class UAnimMontage;
class UBoxComponent;
class USoundCue;
class UAudioComponent;

UCLASS()
class DWARFWARS_API ADwarfCharacter : public ACharacter {
	GENERATED_BODY()

public:
	ADwarfCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void PunchAttackStart();
	void PunchAttackEnd();

protected:
	virtual void BeginPlay() override;

	/* Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputMoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputLookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputPauseAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputPunchAction;

	
	/* Movement */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Pause();
	
	/* Triggers Attack Animations Based on User Input */
	void Punch();

private:
	/* Camera */
	UPROPERTY(VisibleAnywhere, Category = "Camera") 
	UCameraComponent* PlayerCamera;

	/* Player Settings */
	UPROPERTY(EditAnywhere, Category = "Player Settings")
	float MouseSensitivity;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* OverheadWidget;*/

	/* Anim Stuff */
	UPROPERTY(Replicated, EditAnywhere, Category = "Animation Stuff", meta = (AllowPrivateAccess = "true"))
	bool bIsFruity;

	UPROPERTY(EditAnywhere, Category = "Animation Stuff", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PunchAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Audio Stuff", meta = (AllowPrivateAccess = "true"))
	USoundCue* PunchSoundCue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightHandCollisionBox;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPunch();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPunch();

	void LocalPunch();

	/* Triggered when collision hit event fires between players */
	UFUNCTION()
	void OnPunchHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/* Used for keeping track of if player is currently mid attack */
	bool bIsAttacking;

	/* Audio */
	UPROPERTY()
	UAudioComponent* PunchAudioComponent;

public:
	/* Getters & Setters */

	UFUNCTION(Exec, Category = "Commands")
	void SetIsFruity(bool bFruity);

	bool IsFruity();

	bool IsMoving();
};
