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

UCLASS()
class DWARFWARS_API ADwarfCharacter : public ACharacter {
	GENERATED_BODY()

public:
	ADwarfCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;

protected:
	virtual void BeginPlay() override;

	/* Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputMoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputLookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputJumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") UInputAction* InputPauseAction;

	/* Movement */
	void Move(const FInputActionValue& Value);
	//void StopMoving();
	void Look(const FInputActionValue& Value);
	virtual void Landed(const FHitResult& Hit) override;
	void Pause();
private:
	/* Camera */
	UPROPERTY(VisibleAnywhere, Category = "Camera") 
	UCameraComponent* PlayerCamera;

	/* Player Settings */
	UPROPERTY(EditAnywhere, Category = "Player Settings")
	float MouseSensitivity;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* OverheadWidget;*/
};
