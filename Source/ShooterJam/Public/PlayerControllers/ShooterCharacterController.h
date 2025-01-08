// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "ShooterCharacterController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AShooterCharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHudHealth(float InHealth, float InMaxHealth);

private:
	class AShooterHUD* ShooterHud;

protected:
	virtual void BeginPlay() override;

public:
	
};
