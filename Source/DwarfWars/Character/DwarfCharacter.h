// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "DwarfCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class UCombatComponent;
class AWeapon;

UCLASS()
class DWARFWARS_API ADwarfCharacter : public ACharacter {
	GENERATED_BODY()

public:
	ADwarfCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

	/* Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputMoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputLookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputPauseAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputJumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputEquipAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputAimAction;
	/* Input Callbacks */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump();
	void Pause();
	void Equip();
	void AimInput(const FInputActionValue& Value);

private:
	/* Camera */
	UPROPERTY(VisibleAnywhere, Category = "Camera") 
	UCameraComponent* PlayerCamera;

	/* Player Settings */
	UPROPERTY(EditAnywhere, Category = "Player Settings")
	float MouseSensitivity;

	void ScreenLog(const FString& TextToLog);
	void DebugLog(const FString& TextToLog);

	UPROPERTY(VisibleAnywhere)
	UCombatComponent* CombatComponent;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* PrevWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquipPressed();

public:
	/* Getters & Setters */
	
	// For custom commands
	//UFUNCTION(Exec, Category = "Commands")

	bool IsMoving();

	void SetOverlappingWeapon(AWeapon* Weapon);

	bool IsWeaponEquipped();
	bool IsAiming();

	AWeapon* GetEquippedWeapon();
};
