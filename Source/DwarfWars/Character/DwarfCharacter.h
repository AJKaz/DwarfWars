// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DwarfCharacter.generated.h"

UCLASS()
class DWARFWARS_API ADwarfCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADwarfCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;

protected:
	virtual void BeginPlay() override;

private:	
	

};
