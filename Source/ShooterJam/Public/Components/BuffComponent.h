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

public:
	UBuffComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddHealth(float InHealth, float InHealTime = 0.f);

protected:
	virtual void BeginPlay() override;

	void UpdateHealth(float InDeltaTime);

public:
	void SetCharacter(AShooterCharacter* InCharacter);
};
