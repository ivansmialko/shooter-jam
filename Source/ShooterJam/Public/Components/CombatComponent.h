// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "HUD/ShooterHUD.h"

#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERJAM_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class AShooterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void EquipWeapon(class AWeaponBase* InWeaponToEquip);
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void DropWeapon();

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION(Server, Reliable)
	void Server_FireWeapon(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FireWeapon(const FVector_NetQuantize& TraceHitTarget);

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

protected:
	virtual void BeginPlay() override;

	void SetHUDCrosshairs(float DeltaTime);
	void InterpFov(float DeltaTime);

	void StartFireTimer();
	void OnFireTimerFinished();
	void FireWeapon();

public:
	bool GetIsWeaponEquipped() const;
	bool GetIsAiming() const;
	void SetIsAiming(bool bInIsAiming);
	void SetIsFiring(bool bInIsFiring);
	void SetHitTarget(const FVector& TraceHitTarget);
	AWeaponBase* GetEquippedWeapon() const;
	FVector GetHitTarget() const;
};
