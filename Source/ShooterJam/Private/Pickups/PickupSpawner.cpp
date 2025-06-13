// Made by smialko


#include "Pickups/PickupSpawner.h"
#include "Pickups/Pickup.h"
#include "NiagaraComponent.h"

APickupSpawner::APickupSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SpawnerParticlesComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpawnerParticlesComponent"));
	SpawnerParticlesComponent->SetupAttachment(RootComponent);
}

void APickupSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
		return;

	SpawnPickup();
}

void APickupSpawner::SpawnPickup()
{
	if (!HasAuthority())
		return;

	int32 NumPickupClasses{ PickupClasses.Num() };
	if (NumPickupClasses == 0)
		return;

	int32 RandomIndex{ FMath::RandRange(0, NumPickupClasses - 1) };
	SpawnedPickup = GetWorld()->SpawnActor<APickup>(PickupClasses[RandomIndex], GetActorTransform());

	DisableParticles();

	if (!HasAuthority())
		return;

	if (!SpawnedPickup)
		return;

	SpawnedPickup->OnDestroyed.AddDynamic(this, &APickupSpawner::OnSpawnedPickupDestroyed);
}

void APickupSpawner::StartSpawnPickupTimer()
{
	const float SpawnTime = FMath::FRandRange(SpawnTimeMin, SpawnTimeMax);
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &APickupSpawner::OnSpawnTimerFinished, SpawnTime);
}

void APickupSpawner::OnSpawnTimerFinished()
{
	DisableParticles();

	if (!HasAuthority())
		return;

	SpawnPickup();
}

void APickupSpawner::DisableParticles()
{
	if (!SpawnerParticlesComponent)
		return;

	SpawnerParticlesComponent->SetVisibility(false);
}

void APickupSpawner::EnableParticles()
{
	if (!SpawnerParticlesComponent)
		return;

	SpawnerParticlesComponent->SetVisibility(true);
}

void APickupSpawner::OnSpawnedPickupDestroyed(AActor* DestroyedActor)
{
	StartSpawnPickupTimer();
	EnableParticles();
}

void APickupSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

