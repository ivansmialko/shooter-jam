// Made by smialko


#include "GameModes/ShooterGameMode.h"

#include "Characters/ShooterCharacter.h"
#include "PlayerState/ShooterPlayerState.h"
#include "GameStates/ShooterGameState.h"

#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/ShooterCharacterController.h"
#include "GameFramework/PlayerStart.h"

namespace MatchState
{
	const FName Cooldown = FName(TEXT("Cooldown"));
};

AShooterGameMode::AShooterGameMode()
{
	bDelayedStart = true;
}

void AShooterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetMatchState() == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupDuration - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (GetMatchState() == MatchState::InProgress)
	{
		CountdownTime = WarmupDuration + MatchDuration - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (GetMatchState() == MatchState::Cooldown)
	{
		CountdownTime = CooldownDuration + WarmupDuration + MatchDuration - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void AShooterGameMode::OnPlayerEliminated(AShooterCharacter* InElimCharacter, AShooterCharacterController* InElimController, AShooterCharacterController* InAttackerController)
{
	if (!InAttackerController)
		return;

	if (!InElimController)
		return;

	AShooterGameState* ShooterGameState = GetGameState<AShooterGameState>();
	if (!ShooterGameState)
		return;

	AShooterPlayerState* AttackerPlayerState = InAttackerController->GetPlayerState<AShooterPlayerState>();
	AShooterPlayerState* EliminatedPlayerState = InElimController->GetPlayerState<AShooterPlayerState>();
	if (!AttackerPlayerState || !EliminatedPlayerState)
		return;

	TArray<AShooterPlayerState*> PlayersCurrentlyInTheLead{ ShooterGameState->GetTopScoringPlayers() };

	AttackerPlayerState->UpdateScore(AttackerPlayerState->GetScore() + 1.f);
	EliminatedPlayerState->UpdateDefeats(EliminatedPlayerState->GetDefeats() + 1);

	ShooterGameState->UpdateTopScore(AttackerPlayerState);

	//Reward winner 
	if (ShooterGameState->GetTopScoringPlayers().Contains(AttackerPlayerState))
	{
		if (AShooterCharacter * Winner{ Cast<AShooterCharacter>(AttackerPlayerState->GetPawn()) })
		{
			Winner->Multicast_GainedLead();
		}
	}

	//Punish losers
	for (const auto& Player : PlayersCurrentlyInTheLead)
	{
		if (!ShooterGameState->GetTopScoringPlayers().Contains(Player))
		{
			if (AShooterCharacter * Loser{ Cast<AShooterCharacter>(Player->GetPawn()) })
			{
				Loser->Multicast_LostLead();
			}
		}
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AShooterCharacterController* ShooterController{ Cast<AShooterCharacterController>(*It) };
		if (!ShooterController)
			continue;

		ShooterController->OnEliminationBroadcast(AttackerPlayerState, EliminatedPlayerState);
	}

	if (!InElimCharacter)
		return;

	InElimCharacter->OnEliminated(false);
}

void AShooterGameMode::OnPlayerLeft(AShooterPlayerState* InPlayerLeaving)
{
	//Get server's game state
	AShooterGameState* ShooterGameState = GetGameState<AShooterGameState>();
	if (!ShooterGameState)
		return;

	ShooterGameState->RemovePlayerFromTopScore(InPlayerLeaving);

	AShooterCharacter* ShooterCharacterLeaving{ Cast<AShooterCharacter>(InPlayerLeaving->GetPawn()) };
	if (!ShooterCharacterLeaving)
		return;

	ShooterCharacterLeaving->OnEliminated(true);
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

float AShooterGameMode::CalculateDamage(AController* InAttacker, AController* InAttacked, float BaseDamage)
{
	return BaseDamage;
}

void AShooterGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void AShooterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AShooterCharacterController* ShooterController = Cast<AShooterCharacterController>(*It);
		if (!ShooterController)
			return;

		ShooterController->OnMatchStateSet(MatchState, bIsTeamsMatch);
	}
}
