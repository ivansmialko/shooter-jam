// Made by smialko


#include "Pickups/PickupSpawner.h"
#include "Pickups/Pickup.h"

APickupSpawner::APickupSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void APickupSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnPickup();
}

void APickupSpawner::SpawnPickup()
{
	int32 NumPickupClasses{ PickupClasses.Num() };
	if (NumPickupClasses == 0)
		return;

	int32 RandomIndex{ FMath::RandRange(0, NumPickupClasses - 1) };
	SpawnedPickup = GetWorld()->SpawnActor<APickup>(PickupClasses[RandomIndex], GetActorTransform());

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
	if (!HasAuthority())
		return;

	SpawnPickup();
}

void APickupSpawner::OnSpawnedPickupDestroyed(AActor* DestroyedActor)
{
	StartSpawnPickupTimer();
}

void APickupSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

