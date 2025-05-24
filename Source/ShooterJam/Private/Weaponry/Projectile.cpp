// Made by smialko


#include "Weaponry/Projectile.h"

#include "Characters/ShooterCharacter.h"
#include "Game/ShooterJam.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraSystemInstance.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);
	SetRootComponent(CollisionBox);

	bReplicates = true; 
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (TracerParticles)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			TracerParticles,
			CollisionBox,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition);
	}

	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//PlayHitFx();
	Destroy();
}

void AProjectile::SpawnHitFx()
{
	if (!ImpactParticles)
		return;

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		ImpactParticles,
		GetActorTransform());

	if (!ImpactSound)
		return;

	UGameplayStatics::PlaySoundAtLocation(
		this,
		ImpactSound,
		GetActorLocation());
}

void AProjectile::DealExplosionDamage()
{
	APawn* FiringPawn = GetInstigator();
	if (!FiringPawn)
		return;

	AController* FiringController = Cast<AController>(FiringPawn->GetController());
	if (!FiringController)
		return;

	if (HasAuthority())
	{
		UGameplayStatics::ApplyRadialDamageWithFalloff(
			this,						//World context
			BaseDamage,						//Base damage
			10.f,						//Minimal damage
			GetActorLocation(),			//Origin
			DamageInnerRadius,						//Inner radius
			DamageOuterRadius,						//Outer radius
			1.f,						//Falloff
			UDamageType::StaticClass(),	//Damage type
			TArray<AActor*>(),			//Actors to ignore
			this,						//Damage causer
			FiringController			//Instigator
		);
	}
}

void AProjectile::OnDestroyTimerFinished()
{
	Destroy();
}

void AProjectile::SpawnTrailSystem()
{
	if (!TrailSystem)
		return;

	TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(TrailSystem,
		GetRootComponent(),
		FName(),
		GetActorLocation(),
		GetActorRotation(),
		EAttachLocation::KeepWorldPosition,
		false
	);
}

void AProjectile::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AProjectile::OnDestroyTimerFinished, DestroyTime);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::Destroyed()
{
	SpawnHitFx();
	Super::Destroyed();
}