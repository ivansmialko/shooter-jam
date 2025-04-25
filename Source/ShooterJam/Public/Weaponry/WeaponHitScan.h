// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Weaponry/WeaponBase.h"
#include "WeaponHitScan.generated.h"

class UParticleSystem;

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AWeaponHitScan : public AWeaponBase
{
	GENERATED_BODY()
	
//private members
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Style")
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, Category = "Weapon Style")
	UParticleSystem* BeamParticles;

//public methods
public:
	//~ Begin AWeaponBase Interface
	virtual void Fire() override;
	//~ End AWeaponBase Interface

	void DealDamage(const FHitResult& HitResult);
	void SpawnBeamParticles(const FVector& Start, const FVector& End);
	void SpawnImpactParticles(const FHitResult& HitResult);
};
