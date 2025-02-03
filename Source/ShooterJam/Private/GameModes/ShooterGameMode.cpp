// Made by smialko


#include "GameModes/ShooterGameMode.h"

#include "Characters/ShooterCharacter.h"
#include "PlayerState/ShooterPlayerState.h"

#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/ShooterCharacterController.h"
#include "GameFramework/PlayerStart.h"

AShooterGameMode::AShooterGameMode()
{
	bDelayedStart = true;
}

void AShooterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetMatchState() == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
}

void AShooterGameMode::OnPlayerEliminated(class AShooterCharacter* InElimCharacter, class AShooterCharacterController* InElimController, AShooterCharacterController* InAttackerController)
{
	if (!InAttackerController)
		return;

	AShooterPlayerState* AttackerPlayerState = InAttackerController->GetPlayerState<AShooterPlayerState>();
	AShooterPlayerState* EliminatedPlayerState = InElimController->GetPlayerState<AShooterPlayerState>();
	if ((!AttackerPlayerState || !EliminatedPlayerState)
		|| AttackerPlayerState == EliminatedPlayerState)
		return;

	AttackerPlayerState->UpdateScore(AttackerPlayerState->GetScore() + 1.f);
	EliminatedPlayerState->UpdateDefeats(EliminatedPlayerState->GetDefeats() + 1);

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

void AShooterGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}
