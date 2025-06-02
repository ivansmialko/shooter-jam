// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameModes/ShooterGameMode.h"
#include "TeamsGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API ATeamsGameMode : public AShooterGameMode
{
	GENERATED_BODY()

//public methods
public:
	ATeamsGameMode();

	virtual void PostLogin(APlayerController* InNewPlayer) override;
	virtual void Logout(AController* InExitingPlayer) override;

	virtual void OnPlayerEliminated(AShooterCharacter* InElimCharacter, AShooterCharacterController* InElimController, AShooterCharacterController* InAttackerController) override;

	virtual float CalculateDamage(AController* InAttacker, AController* InAttacked, float BaseDamage) override;

//protected methods
protected:
	virtual void HandleMatchHasStarted() override;

};
