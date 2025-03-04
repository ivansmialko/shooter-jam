// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Weaponry/Projectile.h"
#include "ProjectileGrenade.generated.h"

class USoundCue;

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AProjectileGrenade : public AProjectile
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Projectile Style")
	USoundCue* BounceSound;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

public:
	AProjectileGrenade();

	virtual void Destroyed() override;
};
