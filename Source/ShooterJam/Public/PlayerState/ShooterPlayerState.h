// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "ShooterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AShooterPlayerState : public APlayerState
{
	GENERATED_BODY()
	
//private members
private:
	class AShooterCharacter* Character;
	class AShooterCharacterController* Controller;

//private functions
private:
	void CheckInitMembers();
	void UpdateScoreHud();

//public functions
public:
	//~ Begin APlayerState Interface
	virtual void OnRep_Score() override;
	//~ End APlayerState Interface

	virtual void UpdateScore(float InNewScore);
};
