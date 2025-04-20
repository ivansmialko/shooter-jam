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
	bool bIsShielding{ false };

	UPROPERTY(VisibleAnywhere)
	float ShieldingRate{ 0.f };

	UPROPERTY(VisibleAnywhere)
	float ShieldingTarget{ 0.f };

	UPROPERTY(VisibleAnywhere)
	FTimerHandle SpeedBuffTimer;

	UPROPERTY(VisibleAnywhere)
	FTimerHandle JumpBuffTimer;

	UPROPERTY(VisibleAnywhere)
	float InitialBaseSpeed;

	UPROPERTY(VisibleAnywhere)
	float InitialCrouchSpeed;

	UPROPERTY(VisibleAnywhere)
	float InitialJumpVelocity;

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AddSpeed(float InBaseSpeed, float InCrouchSpeed);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AddJump(float InJumpVelocityBoost);

	void OnSpeedBuffTimerFinished();
	void OnJumpBuffTimerFinished();

public:
	UBuffComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddHealth(float InHealth, float InHealTime = 0.f);
	void AddSpeed(float InBaseSpeed, float InCrouchSpeed, float InDuration);
	void AddJump(float InJumpVelocityBoost, float InDuration);
	void AddShield(float InShield, float InShieldTime = 0.f);

protected:
	virtual void BeginPlay() override;

	void UpdateHealth(float InDeltaTime);
	void UpdateShield(float InDeltaTime);

public:
	void SetCharacter(AShooterCharacter* InCharacter);
	void SetInitialBaseSpeed(float InBaseSpeed);
	void SetInitialCrouchSpeed(float InBaseCrouchSpeed);
	void SetInitialJumpVelocity(float InBaseJumpVelocity);
};
