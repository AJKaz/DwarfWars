// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tracer.generated.h"

UCLASS()
class DWARFWARS_API ATracer : public AActor
{
	GENERATED_BODY()
	
public:	
	ATracer();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:	

};
