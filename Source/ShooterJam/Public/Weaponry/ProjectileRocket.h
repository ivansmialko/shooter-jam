// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Weaponry/Projectile.h"
#include "ProjectileRocket.generated.h"

class URocketMovementComponent;

UCLASS()
class SHOOTERJAM_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Projectile Style")
	USoundCue* ProjectileLoop;

	UPROPERTY(EditAnywhere, Category = "Projectile Style")
	USoundAttenuation* ProjectileLoopAttenuation;

	UPROPERTY(VisibleAnywhere, Category = "Projectile State")
	UAudioComponent* ProjectileLoopComponent;

protected:
	UPROPERTY(VisibleAnywhere)
	URocketMovementComponent* RocketMovementComponent;

protected:
	virtual void BeginPlay() override;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

public:
	AProjectileRocket();

	virtual void Destroyed() override;
};
