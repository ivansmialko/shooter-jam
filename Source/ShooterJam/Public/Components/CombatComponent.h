// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Weaponry/WeaponTypes.h"
#include "HUD/ShooterHUD.h"

#include "CombatComponent.generated.h"


class AShooterCharacter;
class AShooterCharacterController;
class AShooterHUD;
class AWeaponBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERJAM_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

//private fields
private:
	class AShooterCharacter* Character;
	class AShooterCharacterController* CharacterController;
	class AShooterHUD* ShooterHud;
	FHUDPackage HudPackage;
	FVector HitTarget;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeaponBase* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bIsAiming;
	UPROPERTY(Replicated)
	bool bIsFiring;

	//Crosshairs spread
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	//Aim FOV change
	float FovCurrent;
	float FovDefault;
	UPROPERTY(EditAnywhere, Category = Zoom)
	float FovZoomed{ 30.f };
	UPROPERTY(EditAnywhere, Category = Zoom)
	float ZoomInterpSpeed{ 20.f };

	//Automatic fire
	FTimerHandle FireTimerHandler;
	bool bIsCanFire{ true };

	//Ammo carried for the currently equipped weapon
	//Using thing because TMap<> cannot be replicated
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	TMap<EWeaponType, int32> CarriedAmmoMap;

//public methods
public:
	UCombatComponent();
	friend class AShooterCharacter;

	//~ Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface

	//~ Begin UObject Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface

	void EquipWeapon(AWeaponBase* InWeaponToEquip);
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void DropWeapon();

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_CarriedAmmo();

	UFUNCTION(Server, Reliable)
	void Server_FireWeapon(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FireWeapon(const FVector_NetQuantize& TraceHitTarget);

//protected methods
protected:
	virtual void BeginPlay() override;

	void SetHUDCrosshairs(float DeltaTime);
	void InterpFov(float DeltaTime);

	void StartFireTimer();
	void OnFireTimerFinished();
	void FireWeapon();
	bool CheckCanFire();

//public methods
public:
	bool GetIsWeaponEquipped() const;
	bool GetIsAiming() const;
	void SetIsAiming(bool bInIsAiming);
	void SetIsFiring(bool bInIsFiring);
	void SetHitTarget(const FVector& TraceHitTarget);
	AWeaponBase* GetEquippedWeapon() const;
	FVector GetHitTarget() const;
};
