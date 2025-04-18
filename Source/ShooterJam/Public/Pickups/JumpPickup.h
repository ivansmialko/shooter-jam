// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "JumpPickup.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AJumpPickup : public APickup
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	float JumpVelocityBoost{ 200.f };

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
