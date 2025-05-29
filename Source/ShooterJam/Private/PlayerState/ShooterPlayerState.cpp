// Made by smialko


#include "PlayerState/ShooterPlayerState.h"

#include "Characters/ShooterCharacter.h"
#include "PlayerControllers/ShooterCharacterController.h"

#include "Net/UnrealNetwork.h"

void AShooterPlayerState::CheckInitMembers()
{
	if (!Character)
	{
		Character = Cast<AShooterCharacter>(GetPawn());
	}

	if (!ShooterCharacterController && Character)
	{
		ShooterCharacterController = Cast<AShooterCharacterController>(Character->GetController());
	}
} 

void AShooterPlayerState::UpdateScoreHud()
{
	CheckInitMembers();

	if (!ShooterCharacterController)
		return;

	if (!ShooterCharacterController->GetPlayerHud())
		return;

	ShooterCharacterController->GetPlayerHud()->SetScore(GetScore());
}

void AShooterPlayerState::UpdateDefeatsHud()
{
	CheckInitMembers();

	if (!ShooterCharacterController)
		return;

	if (!ShooterCharacterController->GetPlayerHud())
		return;
	
	ShooterCharacterController->GetPlayerHud()->SetDefeats(GetDefeats());
}

void AShooterPlayerState::ChangeTeamType(ETeamType InTeamType)
{
	TeamType = InTeamType;

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	if (!ShooterCharacter)
		return;

	ShooterCharacter->ChangeTeamType(InTeamType);
}

void AShooterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	UpdateScoreHud();
}

void AShooterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AShooterPlayerState, Defeats);
	DOREPLIFETIME(AShooterPlayerState, TeamType);
}

void AShooterPlayerState::OnRep_Defeats()
{
	UpdateDefeatsHud();
}

void AShooterPlayerState::OnRep_TeamType(ETeamType InTeamType)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	if (!ShooterCharacter)
		return;

	ShooterCharacter->ChangeTeamType(InTeamType);
}

void AShooterPlayerState::UpdateScore(float InNewScore)
{
	Super::SetScore(InNewScore);

	CheckInitMembers();
	if (!Character->HasAuthority())
	{
		UpdateScoreHud();
	}
}

void AShooterPlayerState::UpdateDefeats(float InNewDefeats)
{
	Defeats = InNewDefeats;

	CheckInitMembers();
	if (!Character->HasAuthority())
	{
		UpdateDefeatsHud();
	}
}
