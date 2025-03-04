// Made by smialko


#include "Weaponry/ProjectileGrenade.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "GameFramework/ProjectileMovementComponent.h"

void AProjectileGrenade::BeginPlay()
{
	AActor::BeginPlay();

	ProjectileMovement->OnProjectileBounce.AddDynamic(this, &AProjectileGrenade::OnBounce);

	StartDestroyTimer();
	SpawnTrailSystem();
}

void AProjectileGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (!BounceSound)
		return;

	UGameplayStatics::PlaySoundAtLocation(this, BounceSound, GetActorLocation());
}

AProjectileGrenade::AProjectileGrenade()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile movement"));
	ProjectileMovement->SetIsReplicated(true);
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
}

void AProjectileGrenade::Destroyed()
{
	DealExplosionDamage();
	Super::Destroyed();
}
