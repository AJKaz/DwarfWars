// Fill out your copyright notice in the Description page of Project Settings.

#include "DwarfCharacter.h"

// Sets default values
ADwarfCharacter::ADwarfCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ADwarfCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADwarfCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADwarfCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADwarfCharacter::Jump() {
}

