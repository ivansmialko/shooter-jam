// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Weaponry/WeaponHitScan.h"
#include "WeaponMultiHitScan.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AWeaponMultiHitScan : public AWeaponHitScan
{
	GENERATED_BODY()
	
//private fields
protected:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties", meta = (EditCondition = "bUseScatter"))
	uint32 ScatterHitsNumber{ 10 };

//public methods
public:
	AWeaponMultiHitScan();

//protected methods
protected:
	//~ Begin AWeaponBase Interface
	virtual void Fire(const FVector& HitTarget) override;
	//~ End AWeaponBase Interface
};
