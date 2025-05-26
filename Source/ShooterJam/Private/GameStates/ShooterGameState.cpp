// Made by smialko


#include "GameStates/ShooterGameState.h"

#include "PlayerState/ShooterPlayerState.h"

#include "Net/UnrealNetwork.h"

void AShooterGameState::OnRep_TeamRedScore()
{

}

void AShooterGameState::OnRep_TeamBlueScore()
{

}

void AShooterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterGameState, TopScoringPlayers);
	DOREPLIFETIME(AShooterGameState, TeamRedScore);
	DOREPLIFETIME(AShooterGameState, TeamBlueScore);
}

void AShooterGameState::UpdateTopScore(AShooterPlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.IsEmpty())
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
		return;
	}

	if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
		return;
	}

	if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

void AShooterGameState::RemovePlayer(AShooterPlayerState* InPlayerToRemove)
{
	if (!InPlayerToRemove)
		return;

	TopScoringPlayers.Remove(InPlayerToRemove);
}
