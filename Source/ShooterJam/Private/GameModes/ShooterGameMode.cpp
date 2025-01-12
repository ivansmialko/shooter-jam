// Made by smialko


#include "GameModes/ShooterGameMode.h"
#include "Characters/ShooterCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/ShooterCharacterController.h"
#include "GameFramework/PlayerStart.h"

void AShooterGameMode::OnPlayerEliminated(class AShooterCharacter* InElimCharacter, class AShooterCharacterController* InElimController, AShooterCharacterController* InAttackerController)
{
	if (!InElimCharacter)
		return;

	InElimCharacter->OnEliminated();
}

void AShooterGameMode::RequestRespawn(ACharacter* InCharacter, AController* InController)
{
	if (!InCharacter)
		return;


	InCharacter->Reset();
	InCharacter->Destroy();

	if (!InController)
		return;

	TArray<AActor*> PlayerStartsArray;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStartsArray);
	if (PlayerStartsArray.IsEmpty())
		return;

	int32 RandomIndex = FMath::RandRange(0, PlayerStartsArray.Num() - 1);

	RestartPlayerAtPlayerStart(InController, PlayerStartsArray[RandomIndex]);
}
