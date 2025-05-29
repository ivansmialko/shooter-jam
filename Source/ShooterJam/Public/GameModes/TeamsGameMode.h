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
	virtual void PostLogin(APlayerController* InNewPlayer) override;
	virtual void Logout(AController* InExitingPlayer) override;

	virtual float CalculateDamage(AController* InAttacker, AController* InAttacked, float BaseDamage) override;

//protected methods
protected:
	virtual void HandleMatchHasStarted() override;

};
