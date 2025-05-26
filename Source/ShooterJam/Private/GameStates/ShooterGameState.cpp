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

void AShooterGameState::RemovePlayerFromTopScore(AShooterPlayerState* InPlayerToRemove)
{
	if (!InPlayerToRemove)
		return;

	TopScoringPlayers.Remove(InPlayerToRemove);
}

ETeamType AShooterGameState::DistributePlayerToTeam(AShooterPlayerState* InPlayerToAdd)
{
	if (TeamRed.Num() > TeamBlue.Num())
	{
		AddPlayerToTeam(InPlayerToAdd, ETeamType::ETT_Blue);
		return ETeamType::ETT_Blue;
	}

	if (TeamBlue.Num() > TeamRed.Num())
	{
		AddPlayerToTeam(InPlayerToAdd, ETeamType::ETT_Red);
		return ETeamType::ETT_Red;
	}

	if (TeamRed.Num() == TeamBlue.Num())
	{
		ETeamType RandomTeam{ static_cast<ETeamType>(FMath::RandRange(static_cast<int32>(ETeamType::ETT_NoTeam) + 1, static_cast<int32>(ETeamType::ETT_MAX) - 1)) };
		AddPlayerToTeam(InPlayerToAdd, RandomTeam);
		return RandomTeam;
	}

	return ETeamType::ETT_NoTeam;
}

void AShooterGameState::AddPlayerToTeam(AShooterPlayerState* InPlayerToAdd, ETeamType InTeamType)
{
	if (InTeamType == ETeamType::ETT_Red)
	{
		TeamRed.AddUnique(InPlayerToAdd);
		return;
	}

	if (InTeamType == ETeamType::ETT_Blue)
	{
		TeamBlue.AddUnique(InPlayerToAdd);
		return;
	}
}

void AShooterGameState::RemovePlayerFromTeam(AShooterPlayerState* InPlayerToRemove)
{
	if (TeamRed.Contains(InPlayerToRemove))
	{
		TeamRed.Contains(InPlayerToRemove);
	}

	if (TeamBlue.Contains(InPlayerToRemove))
	{
		TeamBlue.Remove(InPlayerToRemove);
	}
}