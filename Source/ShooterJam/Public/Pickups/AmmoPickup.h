// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "Weaponry/WeaponTypes.h"

#include "AmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AAmmoPickup : public APickup
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	int32 AmmoAmount{ 30 };

	UPROPERTY(EditAnywhere);
	EWeaponType WeaponType;
	
protected:
	virtual void OnSphereOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
};
