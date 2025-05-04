// Made by smialko


#include "Weaponry/WeaponHitScan.h"

#include "Characters/ShooterCharacter.h"
#include "Components/LagCompensationComponent.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

void AWeaponHitScan::SpawnBeamParticles(const FVector& Start, const FVector& End)
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	if (!BeamParticles)
		return;

	UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticles, Start);
	if (!Beam)
		return;

	Beam->SetVectorParameter(FName("Target"), End);
}

void AWeaponHitScan::SpawnImpactParticles(const FHitResult& HitResult)
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	if (!ImpactParticles)
		return;

	UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticles, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
}

void AWeaponHitScan::Fire()
{
	if (HitTargets.IsEmpty())
		return;
	
	FTransform SocketTransform = GetMuzzleTransform();
	FVector Start = SocketTransform.GetLocation();
	FVector End = GetTraceEnd(Start, HitTargets[0]);
	FVector BeamEnd = End;

	FHitResult FireHit;
	HitScan(FireHit, Start, End);

	if (FireHit.bBlockingHit)
	{
		SpawnImpactParticles(FireHit);
		DealDamage(FireHit, Start);

		BeamEnd = FireHit.ImpactPoint;
	}

	DrawDebugSphere(GetWorld(), BeamEnd, 16.f, 12, FColor::Orange, true);

	SpawnBeamParticles(Start, BeamEnd);

	Super::Fire();
}