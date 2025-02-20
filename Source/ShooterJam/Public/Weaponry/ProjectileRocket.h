// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Weaponry/Projectile.h"
#include "ProjectileRocket.generated.h"

class UNiagaraSystem;

UCLASS()
class SHOOTERJAM_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* TrailSystem;

protected:
	virtual void BeginPlay() override;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

public:
	AProjectileRocket();
};
