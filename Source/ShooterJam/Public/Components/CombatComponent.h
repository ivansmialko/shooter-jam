// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Weaponry/WeaponTypes.h"
#include "Weaponry/CombatState.h"
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

	//~ Begin Replicated fields
	/** True if player holding aim button */
	UPROPERTY(Replicated)
	bool bIsAiming;

	/** True if player holding fire button */
	UPROPERTY(Replicated)
	bool bIsFiring;

	/** A weapon, player currently equipped with. Replicates to all machines */
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeaponBase* EquippedWeapon;

	/**
	 * Ammo carried for the currently equipped weapon
	 * Using thing because TMap<EWeaponType, int32> cannot be replicated
	 * Replicates only to owner machine
	 */
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	/** Current state of player's combat, e.g. whether it reloading, or just hanging around, or something else */
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState{ ECombatState::ECS_Unoccupied };
	//~ End Replicated fields

	//~ Begin Exposed fields
	UPROPERTY(EditAnywhere, Category = Zoom)
	float FovZoomed{ 30.f };

	UPROPERTY(EditAnywhere, Category = Zoom)
	float ZoomInterpSpeed{ 20.f };
	//~ End Exposed fields

	class AShooterCharacter* Character;
	class AShooterCharacterController* CharacterController;
	class AShooterHUD* ShooterHud;
	FTimerHandle FireTimerHandler;
	TMap<EWeaponType, int32> CarriedAmmoMap;
	FCrosshairsPackage HudPackage;
	FVector HitTarget;

	//Crosshairs spread
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	//Aim FOV change
	float FovCurrent;
	float FovDefault;

	//Automatic fire
	bool bIsCanFire{ true };

	UPROPERTY(EditAnywhere)
	int32 StartingArAmmo{ 30 };

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo{ 0 };

	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo{ 0 };

	UPROPERTY(EditAnywhere)
	int32 StartingSmgAmmo{ 0 };

	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo{ 0 };

	UPROPERTY(EditAnywhere)
	int32 StartingSniperAmmo{ 0 };

	UPROPERTY(EditAnywhere)
	int32 StartingGranadeLauncherAmmo{ 0 };

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
	void DropWeapon();
	void DropWeaponLaunch();
	void ReloadWeapon();
	void Throw();

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	
	//~ Begin Replication Notifies
	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_CarriedAmmo();

	UFUNCTION()
	void OnRep_CombatState();
	//~ End Replication Notifies

	void OnShellInserted();
	void OnReloadFinished();

	UFUNCTION(Server, Reliable)
	void Server_FireWeapon(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable)
	void Server_Throw();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FireWeapon(const FVector_NetQuantize& TraceHitTarget);


//private methods
private:
	void InitializeCarriedAmmo();
	void UpdateCurrentCarriedAmmo(const EWeaponType WeaponType);

	/** Replenishes ammo inside of weapon.
	 * InBulletsRequested - bullets amount to try to load.
	 * If zero - everything will be calculated automatically
	 */
	void ReloadAmmo(uint32 InBulletsRequested = 0);
	void PlayEquipSound();

	int32 CalculateAmountToReload(uint32 InRequestedAmount = 0);

//protected methods
protected:
	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	//~ End UActorComponent Interface

	void SetHUDCrosshairs(float DeltaTime);
	void InterpFov(float DeltaTime);

	void OnFireTimerFinished();

	void StartFireTimer();
	void FireWeapon();
	bool CheckCanFire();
	bool CheckCanReload();

	void OnStateReload();
	void OnStateThrow();

//public getters/setters
public:
	FORCEINLINE bool GetIsWeaponEquipped() const { return EquippedWeapon != nullptr; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	FORCEINLINE int32 GetCarriedAmmo() const { return CarriedAmmo; }
	FORCEINLINE FVector GetHitTarget() const { return HitTarget; }
	FORCEINLINE AWeaponBase* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool GetIsReloading() const { return CombatState == ECombatState::ECS_Reloading; }

	void SetIsAiming(bool bInIsAiming);
	void SetIsFiring(bool bInIsFiring);
	void SetHitTarget(const FVector& TraceHitTarget);
};
