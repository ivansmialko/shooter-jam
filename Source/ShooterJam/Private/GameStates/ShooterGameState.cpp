// Made by smialko


#include "GameStates/ShooterGameState.h"

#include "PlayerControllers/ShooterCharacterController.h"
#include "PlayerState/ShooterPlayerState.h"
#include "HUD/ShooterHUD.h"
#include "HUD/TeamBattleWidget.h"

#include "Net/UnrealNetwork.h"

void AShooterGameState::OnRep_TeamRedScore()
{
	AShooterCharacterController* ShooterController = Cast<AShooterCharacterController>(GetWorld()->GetFirstPlayerController());
	if (!ShooterController)
		return;

	ShooterController->GetPlayerHud()->GetTeamBattleWidget()->SetRedProgress(TeamRedScore);
}

void AShooterGameState::OnRep_TeamBlueScore()
{
	AShooterCharacterController* ShooterController = Cast<AShooterCharacterController>(GetWorld()->GetFirstPlayerController());
	if (!ShooterController)
		return;

	ShooterController->GetPlayerHud()->GetTeamBattleWidget()->SetBlueProgress(TeamRedScore);
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
		TeamRed.Remove(InPlayerToRemove);
	}

	if (TeamBlue.Contains(InPlayerToRemove))
	{
		TeamBlue.Remove(InPlayerToRemove);
	}
}

void AShooterGameState::TeamScore(ETeamType InTeamType)
{
	AShooterCharacterController* ShooterController = Cast<AShooterCharacterController>(GetWorld()->GetFirstPlayerController());
	if (!ShooterController)
		return;

	switch (InTeamType)
	{
	case ETeamType::ETT_Red:
	{
		++TeamRedScore;
		ShooterController->GetPlayerHud()->GetTeamBattleWidget()->SetRedProgress(TeamRedScore);

	} break;
	case ETeamType::ETT_Blue:
	{
		++TeamBlueScore;
		ShooterController->GetPlayerHud()->GetTeamBattleWidget()->SetBlueProgress(TeamRedScore);

	} break;
	default:
		break;
	}





}
