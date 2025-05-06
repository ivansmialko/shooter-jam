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
	
public:
	virtual void Fire() override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ServerSideRewindClass;

};
