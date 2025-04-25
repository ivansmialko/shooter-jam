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
	
//public methods
public:
	AWeaponMultiHitScan();

//protected methods
protected:
	//~ Begin AWeaponBase Interface

	virtual void Fire() override;

	//~ End AWeaponBase Interface
};
