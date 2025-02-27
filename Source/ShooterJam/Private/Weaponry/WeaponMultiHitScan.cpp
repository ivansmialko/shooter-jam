// Made by smialko


#include "Weaponry/WeaponMultiHitScan.h"

void AWeaponMultiHitScan::Fire(const FVector& HitTarget)
{
	AWeaponBase::Fire(HitTarget);

	FTransform SocketTransform = GetMuzzleTransform();
	FVector Start = SocketTransform.GetLocation();

	for (uint32 i = 0; i < HitsNumber; ++i)
	{
		FVector End = GetTraceEndWithScatter(Start, HitTarget);
	}

	//UWorld* World = GetWorld();
	//if (!World)
	//	return;

	//FHitResult FireHit;
	//World->LineTraceSingleByChannel(FireHit, Start, End, ECollisionChannel::ECC_Visibility);

	//FVector BeamEnd = End;

	//if (FireHit.bBlockingHit)
	//{
	//	SpawnImpactParticles(FireHit);
	//	DealDamage(FireHit);

	//	BeamEnd = FireHit.ImpactPoint;
	//}

	//SpawnBeamParticles(SocketTransform.GetLocation(), BeamEnd);
}
