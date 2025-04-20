// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawner.generated.h"

class APickup;

UCLASS()
class SHOOTERJAM_API APickupSpawner : public AActor
{
	GENERATED_BODY()
	

private:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<APickup>> PickupClasses;

	UPROPERTY(EditAnywhere)
	float SpawnTimeMin{ 5.f };

	UPROPERTY(EditAnywhere)
	float SpawnTimeMax{ 10.f };

	UPROPERTY(VisibleAnywhere)
	FTimerHandle SpawnTimer;

	UPROPERTY(VisibleAnywhere)
	APickup* SpawnedPickup;

public:
	APickupSpawner();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	void SpawnPickup();
	void StartSpawnPickupTimer();

	void OnSpawnTimerFinished();

	UFUNCTION()
	void OnSpawnedPickupDestroyed(AActor* DestroyedActor);
};
