// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DwarfHUD.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FHUDPackage {
	GENERATED_BODY()

public:
	UPROPERTY()
	UTexture2D* CrosshairCenter;
	UPROPERTY()
	UTexture2D* CrosshairTop;
	UPROPERTY()
	UTexture2D* CrosshairBottom;
	UPROPERTY()
	UTexture2D* CrosshairLeft;
	UPROPERTY()
	UTexture2D* CrosshairRight;

	float CrosshairSpread;
};

/**
 * 
 */
UCLASS()
class DWARFWARS_API ADwarfHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;

private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread);

	FLinearColor CrosshairColor = FLinearColor::White;

	UPROPERTY(EditAnywhere)
	float MaxCrosshairSpread = 16.f;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
};
