// Made by smialko


#include "Pickups/WeaponSpawner.h"
#include "Weaponry/WeaponBase.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"

AWeaponSpawner::AWeaponSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SpawnerParticlesComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpawnerParticlesComponent"));
	SpawnerParticlesComponent->SetupAttachment(RootComponent);
}

void AWeaponSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
		return;

	SpawnWeapon();
}

void AWeaponSpawner::SpawnWeapon()
{
	if (!HasAuthority())
		return;

	int32 NumWeaponClasses{ WeaponClasses.Num() };
	if (NumWeaponClasses == 0)
		return;

	int32 RandomIndex{ FMath::RandRange(0, NumWeaponClasses - 1) };
	SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClasses[RandomIndex], GetActorTransform());

	DisableParticles();

	if (!HasAuthority())
		return;

	if (!SpawnedWeapon)
		return;

	SpawnedWeapon->OnWeaponPickedUp.AddDynamic(this, &AWeaponSpawner::OnSpawnerWeaponPickedUp);
}

void AWeaponSpawner::StartWeaponSpawnTimer()
{
	const float SpawnTime = FMath::FRandRange(SpawnTimeMin, SpawnTimeMax);
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &AWeaponSpawner::OnSpawnTimerFinished, SpawnTime);
}

void AWeaponSpawner::DisableParticles()
{
	if (!SpawnerParticlesComponent)
		return;

	SpawnerParticlesComponent->SetVisibility(false);

	bShowParticles = false;
}

void AWeaponSpawner::EnableParticles()
{
	if (!SpawnerParticlesComponent)
		return;

	SpawnerParticlesComponent->SetVisibility(true);

	bShowParticles = true;
}

void AWeaponSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponSpawner, bShowParticles);
}

void AWeaponSpawner::OnSpawnTimerFinished()
{
	if (!HasAuthority())
		return;

	DisableParticles();
	SpawnWeapon();
}

void AWeaponSpawner::OnRep_ShowParticles()
{
	if (bShowParticles)
	{
		EnableParticles();
	}
	else
	{
		DisableParticles();
	}
}

void AWeaponSpawner::OnSpawnerWeaponPickedUp()
{
	if (!HasAuthority())
		return;

	EnableParticles();
	StartWeaponSpawnTimer();
}

void AWeaponSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

