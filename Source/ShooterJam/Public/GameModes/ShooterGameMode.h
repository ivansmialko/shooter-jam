// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ShooterGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AShooterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void OnPlayerEliminated(class AShooterCharacter* InElimCharacter, class AShooterCharacterController* InElimController, AShooterCharacterController* InAttackerController);
	
};
