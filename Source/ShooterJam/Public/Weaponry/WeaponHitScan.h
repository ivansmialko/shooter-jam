// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Weaponry/WeaponBase.h"
#include "WeaponHitScan.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AWeaponHitScan : public AWeaponBase
{
	GENERATED_BODY()
	
//private members
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UParticleSystem* ImpactParticles;

//public methods
public:
	//~ Begin AWeaponBase Interface
	virtual void Fire(const FVector& HitTarget) override;
	//~ End AWeaponBase Interface
};
