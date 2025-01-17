// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "ShooterCharacterController.generated.h"

class AShooterHUD;

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AShooterCharacterController : public APlayerController
{
	GENERATED_BODY()


private:
	AShooterHUD* ShooterHud;

public:

	//~ Begin APlayerController Interface
	void OnPossess(APawn* InPawn) override;
	//~ End APlayerController Interface

	void SetHudHealth(float InHealth, float InMaxHealth);
	void SetHudScore(float InScore);

protected:
	//~ Begin AActor Inteface
	virtual void BeginPlay() override;
	//~ End AActor Interface
};
