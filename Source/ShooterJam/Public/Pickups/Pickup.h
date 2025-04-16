// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

class USphereComponent;
class USoundCue;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class SHOOTERJAM_API APickup : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere)
	USoundCue* PickupSound;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditAnywhere)
	float BaseTurnRate{ 45.f };

protected:
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* PickupEffect;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* PickupEffectComponent;

private:
	void PlayPickupSound();
	void RotateMesh(float InDeltaTime);

protected:
	virtual void BeginPlay() override;

	void PlayPickupFx();

	UFUNCTION()
	virtual void OnSphereOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

public:	
	APickup();

	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
};
