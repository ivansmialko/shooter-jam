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
private:
	UPROPERTY(EditAnywhere)
	uint32 HitsNumber{ 10 };

//protected methods
protected:
	//~ Begin AWeaponBase Interface
	virtual void Fire(const FVector& HitTarget) override;
	//~ End AWeaponBase Interface
};
