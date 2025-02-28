// Made by smialko


#include "Weaponry/WeaponMultiHitScan.h"

AWeaponMultiHitScan::AWeaponMultiHitScan()
{
	bUseScatter = true;
}

void AWeaponMultiHitScan::Fire(const FVector& HitTarget)
{
	AWeaponBase::Fire(HitTarget);

	FTransform SocketTransform = GetMuzzleTransform();
	FVector Start = SocketTransform.GetLocation();
	FVector BeamEnd;

	FHitResult FireHit;
	for (uint32 i = 0; i < ScatterHitsNumber; ++i)
	{
		FVector End = GetTraceEndWithScatter(Start, HitTarget);
		BeamEnd = End;
		HitScan(FireHit, Start, End);

		if (FireHit.bBlockingHit)
		{
			SpawnImpactParticles(FireHit);
			DealDamage(FireHit);
			BeamEnd = End;
		}

		SpawnBeamParticles(Start, BeamEnd);
	}
}
