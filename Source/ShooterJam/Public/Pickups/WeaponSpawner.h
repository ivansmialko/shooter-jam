// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponSpawner.generated.h"

class AWeaponBase;
class UNiagaraComponent;

UCLASS()
class SHOOTERJAM_API AWeaponSpawner : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AWeaponBase>> WeaponClasses;

	UPROPERTY(EditAnywhere)
	float SpawnTimeMin{ 5.f };

	UPROPERTY(EditAnywhere)
	float SpawnTimeMax{ 10.f };

	UPROPERTY(VisibleAnywhere)
	FTimerHandle SpawnTimer;

	UPROPERTY(VisibleAnywhere)
	AWeaponBase* SpawnedWeapon;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* SpawnerParticlesComponent;

	UPROPERTY(ReplicatedUsing = OnRep_ShowParticles)
	bool bShowParticles;

public:
	AWeaponSpawner();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	void SpawnWeapon();
	void StartWeaponSpawnTimer();
	void DisableParticles();
	void EnableParticles();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnSpawnTimerFinished();

	UFUNCTION()
	void OnRep_ShowParticles();

	UFUNCTION()
	void OnSpawnerWeaponPickedUp();
};
