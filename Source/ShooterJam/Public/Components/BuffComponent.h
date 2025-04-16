// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

class AShooterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERJAM_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	AShooterCharacter* Character;

	UPROPERTY(VisibleAnywhere)
	bool bIsHealing{ false };

	UPROPERTY(VisibleAnywhere)
	float HealingRate{ 0.f };

	UPROPERTY(VisibleAnywhere)
	float HealingTarget{ 0.f };

	UPROPERTY(VisibleAnywhere)
	FTimerHandle SpeedBuffTimer;

	UPROPERTY(VisibleAnywhere)
	float InitialBaseSpeed;

	UPROPERTY(VisibleAnywhere)
	float InitialCrouchSpeed;

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AddSpeed(float InBaseSpeed, float InCrouchSpeed);

	void OnSpeedBuffTimerFinished();

public:
	UBuffComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddHealth(float InHealth, float InHealTime = 0.f);
	void AddSpeed(float InBaseSpeed, float InCrouchSpeed, float InDuration);

protected:
	virtual void BeginPlay() override;

	void UpdateHealth(float InDeltaTime);

public:
	void SetCharacter(AShooterCharacter* InCharacter);
	void SetInitialBaseSpeed(float InBaseSpeed);
	void SetInitialCrouchSpeed(float InBaseCrouchSpeed);
};
