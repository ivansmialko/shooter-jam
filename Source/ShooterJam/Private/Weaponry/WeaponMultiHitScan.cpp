// Made by smialko


#include "Weaponry/WeaponMultiHitScan.h"
#include "DrawDebugHelpers.h"

AWeaponMultiHitScan::AWeaponMultiHitScan()
{
	bUseScatter = true;
}

void AWeaponMultiHitScan::Fire()
{
	FTransform SocketTransform = GetMuzzleTransform();
	FVector Start = SocketTransform.GetLocation();
	FVector BeamEnd;

	FHitResult FireHit;
	for (int32 i = 0; i < HitTargets.Num(); ++i)
	{
		FVector End = GetTraceEnd(Start, HitTargets[i]);
		BeamEnd = End;
		HitScan(FireHit, Start, End);

		if (FireHit.bBlockingHit)
		{
			SpawnImpactParticles(FireHit);
			DealDamage(FireHit);
			BeamEnd = End;
		}

		DrawDebugSphere(GetWorld(), BeamEnd, 16.f, 12, FColor::Orange, true);

		SpawnBeamParticles(Start, BeamEnd);
	}

	AWeaponBase::Fire();
}
