// Made by smialko


#include "Weaponry/WeaponHitScan.h"

#include "Characters/ShooterCharacter.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"

void AWeaponHitScan::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!GetWeaponMesh())
		return;

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (!MuzzleFlashSocket)
		return;

	FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	
	FVector Start = SocketTransform.GetLocation();
	FVector End = Start + (HitTarget - Start) * 1.25;

	FHitResult FireHit;
	
	UWorld* World = GetWorld();
	if (!World)
		return;

	World->LineTraceSingleByChannel(FireHit, Start, End, ECollisionChannel::ECC_Visibility);

	if (FireHit.bBlockingHit)
		return;

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(FireHit.GetActor());
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
	UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticles, End, FireHit.ImpactNormal.Rotation());
}
