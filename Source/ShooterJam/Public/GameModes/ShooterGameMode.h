// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ShooterGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AShooterGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime{ 10.f };

	float LevelStartingTime{ 0.f };
	float CountdownTime{ -1.0f };
//public methods
public:
	AShooterGameMode();
	
	virtual void Tick(float DeltaSeconds);

	virtual void OnPlayerEliminated(class AShooterCharacter* InElimCharacter, class AShooterCharacterController* InElimController, AShooterCharacterController* InAttackerController);
	virtual void RequestRespawn(ACharacter* InCharacter, AController* InController);

//protected methods
protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	//~ Begin AGameMode Interface
	virtual void OnMatchStateSet();
	//~ End AGameMode Interface


//public getters/setters
public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
};
