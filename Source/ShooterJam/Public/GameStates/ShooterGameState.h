// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ShooterGameState.generated.h"

class AShooterPlayerState;

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AShooterGameState : public AGameState
{
	GENERATED_BODY()

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

protected:

	UFUNCTION()
	void OnRep_TeamRedScore();

	UFUNCTION()
	void OnRep_TeamBlueScore();

public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void UpdateTopScore(AShooterPlayerState* ScoringPlayer);
	void RemovePlayer(AShooterPlayerState* InPlayerToRemove);

public:
	FORCEINLINE bool IsPlayerLeading(AShooterPlayerState* InPlayerState) const { return TopScoringPlayers.Contains(InPlayerState); }
	FORCEINLINE const TArray<AShooterPlayerState*>& GetTopScoringPlayers() const { return TopScoringPlayers; };
};
