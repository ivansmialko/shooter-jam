// Made by smialko


#include "GameModes/TeamsGameMode.h"
#include "GameStates/ShooterGameState.h"
#include "PlayerState/ShooterPlayerState.h"

#include "Kismet/GameplayStatics.h"

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

		ShooterPlayerState->SetTeamType(ShooterGameState->DistributePlayerToTeam(ShooterPlayerState));
	}
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

	ShooterPlayerState->SetTeamType(ShooterGameState->DistributePlayerToTeam(ShooterPlayerState));
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
