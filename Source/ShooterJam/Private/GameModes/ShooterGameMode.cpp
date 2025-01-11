// Made by smialko


#include "GameModes/ShooterGameMode.h"

#include "Characters/ShooterCharacter.h"
#include "PlayerControllers/ShooterCharacterController.h"

void AShooterGameMode::OnPlayerEliminated(class AShooterCharacter* InElimCharacter, class AShooterCharacterController* InElimController, AShooterCharacterController* InAttackerController)
{
	if (!InElimCharacter)
		return;

	InElimCharacter->OnEliminated();
}
