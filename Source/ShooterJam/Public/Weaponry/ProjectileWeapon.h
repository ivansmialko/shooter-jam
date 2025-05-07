// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Weaponry/WeaponBase.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AProjectileWeapon : public AWeaponBase
{
	GENERATED_BODY()
	
//private members
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ServerSideRewindClass;

//public methods
public:
	virtual void Fire() override;

//protected methods
protected:
	void SpawnProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation);
};
