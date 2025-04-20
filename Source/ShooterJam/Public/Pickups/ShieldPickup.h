// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "ShieldPickup.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AShieldPickup : public APickup
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	float ShieldAmount{ 100.f };

	UPROPERTY(EditAnywhere)
	float ShieldingTime{ 5.f };

protected:
	void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
};
