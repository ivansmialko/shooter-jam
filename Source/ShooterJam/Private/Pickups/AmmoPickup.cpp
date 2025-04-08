// Made by smialko


#include "Pickups/AmmoPickup.h"

#include "Characters/ShooterCharacter.h"
#include "Components/CombatComponent.h"

void AAmmoPickup::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlapBegin(OtherComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, SweepResult);

	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor);
	if (!Character)
		return;

	UCombatComponent* Combat = Character->GetCombatComponent();
	if (!Combat)
		return;

	Combat->PickupAmmo(WeaponType, AmmoAmount);

	Destroy();
}
