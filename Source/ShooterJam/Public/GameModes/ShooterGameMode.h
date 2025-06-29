// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ShooterGameMode.generated.h"

namespace MatchState
{
	extern SHOOTERJAM_API const FName Cooldown; //Match duration has been reached. Display winner and begin cooldown timer
};

class AShooterCharacter;
class AShooterPlayerState;
class AShooterCharacterController;

UCLASS()
class SHOOTERJAM_API AShooterGameMode : public AGameMode
{
	GENERATED_BODY()

//private fields
private:

	/** Duration of pre-match warmup screen with flying mode */
	UPROPERTY(EditDefaultsOnly)
	float WarmupDuration{ 10.f };
	/** Duration of match */
	UPROPERTY(EditDefaultsOnly)
	float MatchDuration{ 120.f };
	/** Duration of after-match cooldown */
	UPROPERTY(EditDefaultsOnly)
	float CooldownDuration{ 10.f };

	/** UNIX time when level has been loaded */
	float LevelStartingTime{ 0.f };
	float CountdownTime{ -1.0f };

//protected fields
protected:
	/** Match mode selector */
	bool bIsTeamsMatch{ false };

//public methods
public:
	AShooterGameMode();
	
	virtual void Tick(float DeltaSeconds);

	virtual void OnPlayerEliminated(AShooterCharacter* InElimCharacter, AShooterCharacterController* InElimController, AShooterCharacterController* InAttackerController);
	virtual void OnPlayerLeft(AShooterPlayerState* InPlayerLeaving);
	virtual void RequestRespawn(ACharacter* InCharacter, AController* InController);

	virtual float CalculateDamage(AController* InAttacker, AController* InAttacked, float BaseDamage);

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
	FORCEINLINE float GetWarmupDuration() const { return WarmupDuration; }
	FORCEINLINE float GetMatchDuration() const { return MatchDuration; }
	FORCEINLINE float GetCooldownDuration() const { return CooldownDuration; }
	FORCEINLINE float GetLevelStartingTime() const { return LevelStartingTime; }
};
