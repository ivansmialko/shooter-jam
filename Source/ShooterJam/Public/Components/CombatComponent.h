// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Weaponry/WeaponBase.h"
#include "Weaponry/WeaponTypes.h"
#include "Weaponry/CombatState.h"
#include "HUD/ShooterHUD.h"

#include "CombatComponent.generated.h"


class AShooterCharacter;
class AShooterCharacterController;
class AShooterHUD;
class AProjectile;

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
	
	/** A weapon, player currently carries on backpack. Replicates to all machines */
	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	AWeaponBase* SecondaryWeapon;

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

	UPROPERTY(EditAnywhere, Category = Grenades)
	TSubclassOf<AProjectile> GrenadeProjectileClass;

	//Grenades implementation
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_GrenadesAmount, Category = Grenades)
	int32 GrenadesAmount;

	UPROPERTY(EditAnywhere, Category = "Starting ammo")
	int32 GrenadesAmountMax;

	UPROPERTY(EditAnywhere, Category = "Starting ammo")
	int32 StartingArAmmo{ 30 };

	UPROPERTY(EditAnywhere, Category = "Starting ammo")
	int32 StartingRocketAmmo{ 0 };

	UPROPERTY(EditAnywhere, Category = "Starting ammo")
	int32 StartingPistolAmmo{ 0 };

	UPROPERTY(EditAnywhere, Category = "Starting ammo")
	int32 StartingSmgAmmo{ 0 };

	UPROPERTY(EditAnywhere, Category = "Starting ammo")
	int32 StartingShotgunAmmo{ 0 };

	UPROPERTY(EditAnywhere, Category = "Starting ammo")
	int32 StartingSniperAmmo{ 0 };

	UPROPERTY(EditAnywhere, Category = "Starting ammo")
	int32 StartingGranadeLauncherAmmo{ 0 };

	UPROPERTY(EditAnywhere)
	float HitTargetOffset{ 100.f };

	UPROPERTY(EditAnywhere)
	float WeaponDropImpulse{ 500.f };
//~ End Exposed fields

	AShooterCharacter* Character;
	AShooterCharacterController* CharacterController;
	AShooterHUD* ShooterHud;
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
	void SwapWeapons();
	void DropWeapon();
	void DropWeaponLaunch();
	void ReloadWeapon();
	void Throw();
	void PickupAmmo(EWeaponType InWeaponType, int32 InAmmoAmount);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	
	//~ Begin Replication Notifies
	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_SecondaryWeapon();

	UFUNCTION()
	void OnRep_CarriedAmmo();

	UFUNCTION()
	void OnRep_CombatState();

	UFUNCTION()
	void OnRep_GrenadesAmount();
	//~ End Replication Notifies

	void OnShellInserted();
	void OnReloadFinished();
	void OnThrowFinished();
	void OnThrowLaunched();

	UFUNCTION(Server, Reliable)
	void Server_FireWeapon(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable)
	void Server_Throw();

	UFUNCTION(Server, Reliable)
	void Server_ThrowGrenade(const FVector_NetQuantize& Target);

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

	/** Attaches an actor to player's right hand */
	void AttachActorToRightHand(AActor* InActor);

	/** Attaches an actor to player's left hand */
	void AttachActorToLeftHand(AActor* InActor);

	/** Attaches an actor to player's backpack slot */
	void AttachActorToBackpack(AActor* InActor);

	/** Shows or hides a grenade attached to player's hand */
	void SetGrenadeVisibility(bool bVisible);

	void PlayEquipSound(AWeaponBase* WeaponToEquip);

	void UpdateHudGrenades();
	void UpdateHudCrosshairs(float DeltaTime);

	int32 CalculateAmountToReload(uint32 InRequestedAmount = 0);

//protected methods
protected:
	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	//~ End UActorComponent Interface

	void InterpFov(float DeltaTime);

	void StartFireTimer();
	void OnFireTimerFinished();

	void RequestFire();
	void ActionFire(const FVector_NetQuantize& TraceHitTarget);
	void RequestThrow();

	bool CheckCanFire();
	bool CheckCanReload();

	void OnStateReload();
	void OnStateThrow();

	void EquipPrimaryWeapon(AWeaponBase* InWeaponToEquip, bool bInDropPrevious = false);
	void EquipSecondaryWeapon(AWeaponBase* InWeaponToEquip);

//public getters/setters
public:
	FORCEINLINE bool GetIsWeaponEquipped() const { return EquippedWeapon != nullptr; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	FORCEINLINE bool GetIsReloading() const { return CombatState == ECombatState::ECS_Reloading; }
	FORCEINLINE bool GetIsThrowing() const { return CombatState == ECombatState::ECS_Throwing; }
	FORCEINLINE bool GetIsUnoccupied() const { return CombatState == ECombatState::ECS_Unoccupied; }
	FORCEINLINE bool GetIsShouldSwapWeapons() const { return (EquippedWeapon != nullptr && SecondaryWeapon != nullptr); }
	FORCEINLINE int32 GetCarriedAmmo() const { return CarriedAmmo; }
	FORCEINLINE int32 GetWeaponAmmo() const { return (EquippedWeapon ? EquippedWeapon->GetWeaponAmmo() : 0); }
	FORCEINLINE int32 GetGrenadesAmount() const { return GrenadesAmount; }
	FORCEINLINE FVector GetHitTarget() const { return HitTarget; }
	FORCEINLINE AWeaponBase* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE AWeaponBase* GetSecondaryWeapon() const { return SecondaryWeapon; }


	void SetIsAiming(bool bInIsAiming);
	void SetIsFiring(bool bInIsFiring);
	void SetHitTarget(const FVector& TraceHitTarget);
};
