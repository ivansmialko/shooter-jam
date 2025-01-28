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

//private variables
private:
	AShooterHUD* ShooterHud;

//private methods
private:
	bool CheckInitHud();

//protected methods
protected:
	//~ Begin AActor Inteface
	virtual void BeginPlay() override;
	//~ End AActor Interface

//public methods
public:

	//~ Begin APlayerController Interface
	void OnPossess(APawn* InPawn) override;
	//~ End APlayerController Interface

	void SetHudHealth(float InHealth, float InMaxHealth);
	void SetHudScore(float InScore);
	void SetHudDefeats(int32 InDefeats);
	void SetHudWeaponAmmo(int32 InAmmo);
	void SetHudCarriedAmmoEmpty();
	void SetHudCarriedAmmo(int32 InAmmo);
	void SetHudWeaponAmmoEmpty();
};
