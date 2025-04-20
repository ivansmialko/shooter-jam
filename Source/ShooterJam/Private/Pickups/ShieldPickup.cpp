// Made by smialko


#include "Pickups/ShieldPickup.h"
#include "Characters/ShooterCharacter.h"
#include "Components/BuffComponent.h"

void AShieldPickup::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlapBegin(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, SweepResult);

	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor);
	if (!Character)
		return;

	UBuffComponent* BuffComponent = Character->GetBuffComponent();
	if (!BuffComponent)
		return;

	BuffComponent->AddShield(ShieldAmount, ShieldingTime);

	Destroy();
}
