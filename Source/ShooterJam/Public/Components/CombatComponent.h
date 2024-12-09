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

	bool GetIsWeaponEquipped();
	bool GetIsAiming();
	AWeaponBase* GetEquippedWeapon() const;

	void SetIsAiming(bool bInIsAiming);
	void SetIsFiring(bool bInIsFiring);
	void SetHitTarget(const FVector& TraceHitTarget);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

private:
	class AShooterCharacter* Character;
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeaponBase* EquippedWeapon;
	UPROPERTY(Replicated)
	bool bIsAiming;
	UPROPERTY(Replicated)
	bool bIsFiring;

	FVector HitTarget;

protected:
	virtual void BeginPlay() override;

public:
};
