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

	float TopScore{ 0 };

public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void UpdateTopScore(AShooterPlayerState* ScoringPlayer);
	void RemovePlayer(AShooterPlayerState* InPlayerToRemove);

public:
	FORCEINLINE bool IsPlayerLeading(AShooterPlayerState* InPlayerState) const { return TopScoringPlayers.Contains(InPlayerState); }
	FORCEINLINE const TArray<AShooterPlayerState*>& GetTopScoringPlayers() const { return TopScoringPlayers; };
};
