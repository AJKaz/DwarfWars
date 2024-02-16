// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "DwarfCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;

UCLASS()
class DWARFWARS_API ADwarfCharacter : public ACharacter {
	GENERATED_BODY()

public:
	ADwarfCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	/* Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputMoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputLookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputPauseAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputJumpAction;

	
	/* Movement */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump();
	void Pause();

private:
	/* Camera */
	UPROPERTY(VisibleAnywhere, Category = "Camera") 
	UCameraComponent* PlayerCamera;

	/* Player Settings */
	UPROPERTY(EditAnywhere, Category = "Player Settings")
	float MouseSensitivity;

	void ScreenLog(const FString& TextToLog);
	void DebugLog(const FString& TextToLog);

public:
	/* Getters & Setters */
	
	//UFUNCTION(Exec, Category = "Commands")	// For custom commands

	bool IsMoving();
};
