// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USphereComponent;
class UWidgetComponent;
class UAnimationAsset;
class UTexture2D;

UENUM(BlueprintType)
enum class EWeaponState : uint8 {
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	
	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class DWARFWARS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ShowPickupWidget(bool bShowWidget);

	virtual void Shoot(const FVector_NetQuantize& StartPos, const FVector_NetQuantize& Direction);

	/* Crosshair Textures */
	UPROPERTY(EditAnywhere, Category = Crosshair)
	UTexture2D* CrosshairCenter;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	UTexture2D* CrosshairTop;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	UTexture2D* CrosshairBottom;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	UTexture2D* CrosshairLeft;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	UTexture2D* CrosshairRight;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	/* 3rd Person View of weapon mesh */
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* Mesh3P;

	/* 1st Person View of weapon mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	/* Collision used to detect character overlap */
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UAnimationAsset* ShootAnimation;

public:
	void SetWeaponState(EWeaponState State);
	USkeletalMeshComponent* GetWeaponMesh(bool bMesh1P) const;
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
};
