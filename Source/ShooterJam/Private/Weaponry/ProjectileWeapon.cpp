// Made by smialko
#include "Weaponry/ProjectileWeapon.h"
#include "Weaponry/Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire()
{
	if (HitTargets.IsEmpty())
		return;

	if (!ProjectileClass)
		return;

	USkeletalMeshComponent* Mesh{ GetWeaponMesh() };
	if (!Mesh)
		return;

	const USkeletalMeshSocket* MuzzleSocket{ Mesh->GetSocketByName(FName("MuzzleFlash")) };
	if (!MuzzleSocket)
		return;

	FTransform MuzzleTransform{ MuzzleSocket->GetSocketTransform(Mesh) };
	FVector ToTarget{ HitTargets[0] - MuzzleTransform.GetLocation()}; //Find vector (direction) from barrel to hit target
	FRotator TargetRotation{ ToTarget.Rotation() };				   //Rotation that represents ToTarget's direction

	UWorld* World{ GetWorld() };
	if (!World)
		return;

	SpawnProjectile(MuzzleTransform.GetLocation(), TargetRotation);

	Super::Fire();
}

void AProjectileWeapon::SpawnProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	UWorld* World{ GetWorld() };
	if (!World)
		return;

	APawn* InstigatorPawn{ Cast<APawn>(GetOwner()) };
	if (!InstigatorPawn)
		return;

	FActorSpawnParameters BulletSpawnParameters;
	BulletSpawnParameters.Owner = GetOwner();
	BulletSpawnParameters.Instigator = InstigatorPawn;

	AProjectile* SpawnedProjectile{ nullptr };
	if (bUseServerSideRewind)
	{
		if (InstigatorPawn->HasAuthority())	//Server
		{
			if (InstigatorPawn->IsLocallyControlled()) //Server, host - spawn replicated projectile, no SSR
			{
				SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, BulletSpawnParameters);
				SpawnedProjectile->SetUseSsr(false);
			}
			else //Server, not locally controlled - spawn non-replicated projectile, use SSR
			{
				SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindClass, SpawnLocation, SpawnRotation, BulletSpawnParameters);
				SpawnedProjectile->SetUseSsr(true);
			}
		}
		else //Client, using SSR
		{
			if (InstigatorPawn->IsLocallyControlled()) //Client, locally controlled - spawn non-replicated projectile, use SSR
			{
				SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindClass, SpawnLocation, SpawnRotation, BulletSpawnParameters);
				SpawnedProjectile->SetUseSsr(true);
				SpawnedProjectile->SetTraceStart(SpawnLocation);
				SpawnedProjectile->SetInitialVelocity(SpawnedProjectile->GetActorForwardVector() * SpawnedProjectile->GetInitialVelocity());
			}
			else //Client, not locally controlled - spawn non-replicated projectile, no SSR
			{
				SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindClass, SpawnLocation, SpawnRotation, BulletSpawnParameters);
				SpawnedProjectile->SetUseSsr(false);
			}
		}
	}
	else //Weapon not using ssr
	{
		if (InstigatorPawn->HasAuthority()) //Server
		{
			SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, BulletSpawnParameters);
			SpawnedProjectile->SetUseSsr(false);
		}
	}

	//Projectile's damage should be equal to weapon's damage, or it's easy to get lost in settings
	SpawnedProjectile->SetDamage(BaseDamage);
}
