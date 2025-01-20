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

	if (!Controller && Character)
	{
		Controller = Cast<AShooterCharacterController>(Character->GetController());
	}
} 

void AShooterPlayerState::UpdateScoreHud()
{
	CheckInitMembers();
	if (!Controller)
		return;

	Controller->SetHudScore(GetScore());
}

void AShooterPlayerState::UpdateDefeatsHud()
{
	CheckInitMembers();
	if (!Controller)
		return;

	Controller->SetHudScore(GetScore());
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

	SetScore(InNewScore);
	if (Character->HasAuthority())
	{
		UpdateScoreHud();
	}
}

void AShooterPlayerState::UpdateDefeats(float InNewDefeats)
{
	Defeats = InNewDefeats;
	if (!Character->HasAuthority())
	{
		UpdateDefeatsHud();
	}
}
