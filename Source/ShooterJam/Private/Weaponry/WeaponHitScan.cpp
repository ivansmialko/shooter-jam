// Made by smialko


#include "Weaponry/WeaponHitScan.h"

#include "Characters/ShooterCharacter.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

void AWeaponHitScan::DealDamage(const FHitResult& HitResult)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(HitResult.GetActor());
	if (!ShooterCharacter)
		return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
		return;

	AController* InstigatorController = OwnerPawn->GetController();
	if (!InstigatorController)
		return;

	if (HasAuthority())
	{
		UGameplayStatics::ApplyDamage(ShooterCharacter, BaseDamage, InstigatorController, this, UDamageType::StaticClass());
	}
}

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

void AWeaponHitScan::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	FTransform SocketTransform = GetMuzzleTransform();
	FVector Start = SocketTransform.GetLocation();
	FVector End = (bUseScatter ? GetTraceEndWithScatter(Start, HitTarget) :  GetTraceEnd(Start, HitTarget));
	FVector BeamEnd = End;

	FHitResult FireHit;
	HitScan(FireHit, Start, End);

	if (FireHit.bBlockingHit)
	{
		SpawnImpactParticles(FireHit);
		DealDamage(FireHit);

		BeamEnd = FireHit.ImpactPoint;
	}

	SpawnBeamParticles(Start, BeamEnd);
}
