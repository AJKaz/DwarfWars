// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfHUD.h"


void ADwarfHUD::DrawHUD() {
	Super::DrawHUD();

	// Draw Crosshairs
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = MaxCrosshairSpread * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairCenter) {
			DrawCrosshair(HUDPackage.CrosshairCenter, ViewportCenter, FVector2D(0.f, 0.f));
		}
		if (HUDPackage.CrosshairTop) {
			DrawCrosshair(HUDPackage.CrosshairTop, ViewportCenter, FVector2D(0.f, -SpreadScaled));
		}
		if (HUDPackage.CrosshairBottom) {
			DrawCrosshair(HUDPackage.CrosshairBottom, ViewportCenter, FVector2D(0.f, SpreadScaled));
		}
		if (HUDPackage.CrosshairLeft) {
			DrawCrosshair(HUDPackage.CrosshairLeft, ViewportCenter, FVector2D(-SpreadScaled, 0.f));
		}
		if (HUDPackage.CrosshairRight) {
			DrawCrosshair(HUDPackage.CrosshairRight, ViewportCenter, FVector2D(SpreadScaled, 0.f));
		}
	}
}

void ADwarfHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread) {
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();

	const FVector2D TextureDrawPoint(ViewportCenter.X - (TextureWidth * 0.5f) + Spread.X, ViewportCenter.Y - (TextureHeight * 0.5f) + Spread.Y);

	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f, 1.f, CrosshairColor);
}
