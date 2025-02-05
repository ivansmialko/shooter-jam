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

	if (!PlayerController && Character)
	{
		PlayerController = Cast<AShooterCharacterController>(Character->GetController());
	}
} 

void AShooterPlayerState::UpdateScoreHud()
{
	CheckInitMembers();

	if (!PlayerController)
		return;

	if (!PlayerController->GetPlayerHud())
		return;

	PlayerController->GetPlayerHud()->SetScore(GetScore());
}

void AShooterPlayerState::UpdateDefeatsHud()
{
	CheckInitMembers();

	if (!PlayerController)
		return;

	if (!PlayerController->GetPlayerHud())
		return;
	
	PlayerController->GetPlayerHud()->SetDefeats(GetDefeats());
}

void AShooterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	UpdateScoreHud();
}

void AShooterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AShooterPlayerState, Defeats);
}

void AShooterPlayerState::OnRep_Defeats()
{
	UpdateDefeatsHud();
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
