// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "SpeedPickup.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API ASpeedPickup : public APickup
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	float BaseSpeedBuff { 1600.f };

	UPROPERTY(EditAnywhere)
	float CrouchSpeedBuff{ 850.f };

	UPROPERTY(EditAnywhere)
	float Duration{ 30.f };

protected:
	void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
};
