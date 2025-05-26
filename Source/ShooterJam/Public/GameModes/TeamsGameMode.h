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

protected:
	virtual void HandleMatchHasStarted() override;
	
public:
	virtual void PostLogin(APlayerController* InNewPlayer) override;
	virtual void Logout(AController* InExitingPlayer) override;
};
