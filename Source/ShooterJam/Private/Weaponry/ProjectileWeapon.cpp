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
				SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, MuzzleTransform.GetLocation(), TargetRotation, BulletSpawnParameters);
				SpawnedProjectile->SetUseSsr(false);
			}
			else //Server, not locally controlled - spawn non-replicated projectile, no SSR
			{
				SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindClass, MuzzleTransform.GetLocation(), TargetRotation, BulletSpawnParameters);
				SpawnedProjectile->SetUseSsr(false);
			}
		}
		else //Client, using SSR
		{
			if (InstigatorPawn->IsLocallyControlled()) //Client, locally controlled - spawn non-replicated projectile, use SSR
			{
				SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindClass, MuzzleTransform.GetLocation(), TargetRotation, BulletSpawnParameters);
				SpawnedProjectile->SetUseSsr(true);
				SpawnedProjectile->SetTraceStart(MuzzleTransform.GetLocation());
				SpawnedProjectile->SetInitialVelocity(SpawnedProjectile->GetActorForwardVector() * SpawnedProjectile->GetInitialVelocity())
			}
		}
	}
	else
	{

	}


	Super::Fire();
}
