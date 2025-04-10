// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "HealthPickup.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class SHOOTERJAM_API AHealthPickup : public APickup
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	float HealAmount{ 100.f };

	UPROPERTY(EditAnywhere)
	float HealTime{ 5.f };

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* PickupEffect;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* PickupEffectComponent;

public:
	AHealthPickup();

	virtual void Destroyed() override;

protected:
	void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

};
