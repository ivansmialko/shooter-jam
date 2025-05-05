// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Weaponry/Projectile.h"
#include "ProjectileBullet.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AProjectileBullet : public AProjectile
{
	GENERATED_BODY()
	
protected:
	//~ Begin AProjectile Interface
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	//~ End AProjectile Interface

	virtual void BeginPlay() override;

public:
	AProjectileBullet();
};
