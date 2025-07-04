// Made by smialko


#include "GameModes/TeamsGameMode.h"
#include "GameStates/ShooterGameState.h"
#include "PlayerState/ShooterPlayerState.h"
#include "PlayerControllers/ShooterCharacterController.h"

#include "Kismet/GameplayStatics.h"

ATeamsGameMode::ATeamsGameMode()
{
	bIsTeamsMatch = true;
}

void ATeamsGameMode::PostLogin(APlayerController* InNewPlayer)
{
	Super::PostLogin(InNewPlayer);

	AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
	if (!ShooterGameState)
		return;

	AShooterPlayerState* ShooterPlayerState = InNewPlayer->GetPlayerState<AShooterPlayerState>();
	if (!ShooterPlayerState)
		return;

	if (ShooterPlayerState->GetTeamType() != ETeamType::ETT_NoTeam)
		return;

	ShooterPlayerState->ChangeTeamType(ShooterGameState->DistributePlayerToTeam(ShooterPlayerState));
}

void ATeamsGameMode::Logout(AController* InExitingPlayer)
{
	AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
	if (!ShooterGameState)
		return;

	AShooterPlayerState* ShooterPlayerState = InExitingPlayer->GetPlayerState<AShooterPlayerState>();
	if (!ShooterPlayerState)
		return;

	ShooterGameState->RemovePlayerFromTeam(ShooterPlayerState);
}

void ATeamsGameMode::OnPlayerEliminated(AShooterCharacter* InElimCharacter, AShooterCharacterController* InElimController, AShooterCharacterController* InAttackerController)
{
	if (!InAttackerController)
		return;

	Super::OnPlayerEliminated(InElimCharacter, InElimController, InAttackerController);

	AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
	if (!ShooterGameState)
		return;

	AShooterPlayerState* AttackerPlayerState = InAttackerController->GetPlayerState<AShooterPlayerState>();
	if (!AttackerPlayerState)
		return;

	ShooterGameState->TeamScore(AttackerPlayerState->GetTeamType());
}

float ATeamsGameMode::CalculateDamage(AController* InAttacker, AController* InAttacked, float BaseDamage)
{
	AShooterPlayerState* AttackerPlayerState{ InAttacker->GetPlayerState<AShooterPlayerState>() };
	AShooterPlayerState* AttackedPlayerState{ InAttacked->GetPlayerState<AShooterPlayerState>() };

	if (!AttackedPlayerState || !AttackedPlayerState)
		return BaseDamage;

	if (AttackerPlayerState == AttackedPlayerState)
		return BaseDamage;

	if (AttackerPlayerState->GetTeamType() == AttackedPlayerState->GetTeamType())
		return 0;

	return BaseDamage;
}

void ATeamsGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
	if (!ShooterGameState)
		return;

	for (const auto& PlayerState : ShooterGameState->PlayerArray)
	{
		AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState.Get());
		if (!ShooterPlayerState)
			continue;

		if (ShooterPlayerState->GetTeamType() != ETeamType::ETT_NoTeam)
			continue;

		ShooterPlayerState->ChangeTeamType(ShooterGameState->DistributePlayerToTeam(ShooterPlayerState));
	}
}