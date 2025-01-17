// Made by smialko


#include "PlayerState/ShooterPlayerState.h"

#include "Characters/ShooterCharacter.h"
#include "PlayerControllers/ShooterCharacterController.h"

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

void AShooterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
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
