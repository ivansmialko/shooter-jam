// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "RocketMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API URocketMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()
	
protected:

	//~ Begin UProjectileMovementComponent Interface
	virtual EHandleBlockingHitResult HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining) override;
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice = 0.f, const FVector& MoveDelta = FVector::ZeroVector) override;
	//~ End UProjectileMovementComponent Interface
};
