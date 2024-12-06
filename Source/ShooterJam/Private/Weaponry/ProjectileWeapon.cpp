// Made by smialko
#include "Weaponry/ProjectileWeapon.h"
#include "Weaponry/Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!ProjectileClass)
		return;

	USkeletalMeshComponent* Mesh{ GetWeaponMesh() };
	if (!Mesh)
		return;

	const USkeletalMeshSocket* MuzzleSocket{ Mesh->GetSocketByName(FName("MuzzleFlash")) };
	if (!MuzzleSocket)
		return;

	FTransform MuzzleTransform{ MuzzleSocket->GetSocketTransform(Mesh) };
	FVector ToTarget{ HitTarget - MuzzleTransform.GetLocation() }; //Find vector (direction) from barrel to hit target
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

	World->SpawnActor<AProjectile>(ProjectileClass, MuzzleTransform.GetLocation(), TargetRotation, BulletSpawnParameters);
}
