// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Projectile.generated.h"

class USoundCue;
class UBoxComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class UParticleSystemComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class SHOOTERJAM_API AProjectile : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Projectile Properties")
	float Damage{ 1.f };

	UPROPERTY(EditAnywhere, Category = "Projectile Properties")
	float DestroyTime{ 3.f };

	UPROPERTY(EditAnywhere, Category = "Projectile Properties")
	float DamageInnerRadius{ 200.f };

	UPROPERTY(EditAnywhere, Category = "Projectile Properties")
	float DamageOuterRadius{ 500.f };

	UPROPERTY(EditAnywhere, Category = "Projectile Properties")
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, Category = "Projectile Style")
	UParticleSystem* TracerParticles;

	UPROPERTY(EditAnywhere, Category = "Projectile Style")
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, Category = "Projectile Style")
	UNiagaraSystem* TrailSystem;

	UPROPERTY(EditAnywhere, Category = "Projectile Style")
	USoundCue* ImpactSound;

	UPROPERTY(VisibleAnywhere, Category = "Projectile State")
	UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(VisibleAnywhere, Category = "Projectile State")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, Category = "Projectile State")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, Category = "Projectile State")
	UParticleSystemComponent* TracerComponent;

	FTimerHandle DestroyTimer;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SpawnHitFx();
	void SpawnTrailSystem();

	void StartDestroyTimer();
	void DealExplosionDamage();

	void OnDestroyTimerFinished();

public:
	AProjectile();

	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
};
