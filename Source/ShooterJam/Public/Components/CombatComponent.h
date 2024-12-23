// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

	UFUNCTION()
	void OnRep_EquippedWeapon();

private:
	class AShooterCharacter* Character;
	class AShooterCharacterController* CharacterController;
	class AShooterHUD* ShooterHUD;
	FVector HitTarget;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeaponBase* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bIsAiming;

	UPROPERTY(Replicated)
	bool bIsFiring;

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;

protected:
	virtual void BeginPlay() override;

	void SetHUDCrosshairs(float DeltaTime);

public:
	bool GetIsWeaponEquipped();
	bool GetIsAiming();
	void SetIsAiming(bool bInIsAiming);
	void SetIsFiring(bool bInIsFiring);
	void SetHitTarget(const FVector& TraceHitTarget);
	AWeaponBase* GetEquippedWeapon() const;
	FVector GetHitTarget() const;
};
