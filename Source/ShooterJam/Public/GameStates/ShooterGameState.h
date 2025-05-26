// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

#include "PlayerState/ShooterPlayerState.h"

#include "ShooterGameState.generated.h"

class AShooterPlayerState;

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AShooterGameState : public AGameState
{
	GENERATED_BODY()

//private members
private:

	UPROPERTY(Replicated)
	TArray<AShooterPlayerState*> TopScoringPlayers;

	TArray<AShooterPlayerState*> TeamRed;
	TArray<AShooterPlayerState*> TeamBlue;

	float TopScore{ 0 };

	UPROPERTY(ReplicatedUsing = OnRep_TeamRedScore)
	float TeamRedScore;
	UPROPERTY(ReplicatedUsing = OnRep_TeamBlueScore)
	float TeamBlueScore;

//protected methods
protected:

	UFUNCTION()
	void OnRep_TeamRedScore();

	UFUNCTION()
	void OnRep_TeamBlueScore();

//public methods
public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void UpdateTopScore(AShooterPlayerState* ScoringPlayer);
	void RemovePlayerFromTopScore(AShooterPlayerState* InPlayerToRemove);

	ETeamType DistributePlayerToTeam(AShooterPlayerState* InPlayerToAdd);
	void AddPlayerToTeam(AShooterPlayerState* InPlayerToAdd, ETeamType InTeamType);
	void RemovePlayerFromTeam(AShooterPlayerState* InPlayerToRemove);

//public getters/setters
public:
	FORCEINLINE bool IsPlayerLeading(AShooterPlayerState* InPlayerState) const { return TopScoringPlayers.Contains(InPlayerState); }
	FORCEINLINE const TArray<AShooterPlayerState*>& GetTopScoringPlayers() const { return TopScoringPlayers; };
	FORCEINLINE const TArray<AShooterPlayerState*>& GetTeamRedMembers() const { return TeamRed; }
	FORCEINLINE const TArray<AShooterPlayerState*>& GetTeamBlueMembers() const { return TeamBlue; }
};
