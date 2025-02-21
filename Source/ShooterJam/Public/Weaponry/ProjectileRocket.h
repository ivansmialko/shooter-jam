// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Weaponry/Projectile.h"
#include "ProjectileRocket.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class URocketMovementComponent;

UCLASS()
class SHOOTERJAM_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	float DestroyTime{ 3.f };

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* TrailSystem;

	UPROPERTY()
	UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(EditAnywhere)
	USoundCue* ProjectileLoop;

	UPROPERTY()
	UAudioComponent* ProjectileLoopComponent;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* ProjectileLoopAttenuation;

	FTimerHandle DestroyTimer;

protected:
	UPROPERTY(VisibleAnywhere)
	URocketMovementComponent* RocketMovementComponent;

protected:
	virtual void BeginPlay() override;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	void DestroyTimerFinished();

public:
	AProjectileRocket();

	virtual void Destroyed() override;
};
