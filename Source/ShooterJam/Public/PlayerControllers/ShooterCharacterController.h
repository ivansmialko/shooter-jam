// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "ShooterCharacterController.generated.h"

class AShooterHUD;
class AShooterCharacter;

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AShooterCharacterController : public APlayerController
{
	GENERATED_BODY()

//private variables
private:

	/** Frequency, controller is requesting servertime with */
	UPROPERTY(EditAnywhere, Category = Time);
	float TimeSyncFrequency{ 5.f };

	/** Reference to player's heads up display */
	AShooterHUD* ShooterHud;

	/** Difference between client and server time */
	float ClientServerDelta{ 0.f };

	/** Timer, controller is using to request servertime with TimeSyncFrequency*/
	float TimeSyncTimer{ TimeSyncFrequency };

	/** Copy of ShooterGameMode's match duration */
	float MatchDuration{ 0.f };
	/** Copy of ShooterGameMode's warmup duration */
	float WarmupDuration{ 0.f };
	/** Copy of ShooterGameMode's time when level started */
	float LevelStartingTime{ 0.f };
	/**  Timer to update warmup/match countdown every second */
	float CountdownTimer{ -1.0f };
	/** Time to update CountdownTimer */
	float CountdownTimerFrequency{ 1.0f };
	
	//Current match state, syncronized with ShooterGameMode's match state
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

//private methods
private:

	/** Sets current match time to HUD */
	void UpdateCountdowns();

	/** Initialized hud variables */
	bool CheckInitHud();

	/** Requests the current server time, passing in the client's time when the request was sent */
	UFUNCTION(Server, Reliable)
	void Server_RequestServerTime(float InTimeOfClientRequest);

	/**  Request game settings, such as match time, warmup time, current match state.
		Used to deliver settings for server's version of a character */
	UFUNCTION(Server, Reliable)
	void Server_RequestGameSettings();

	/**  Same as Server_RequestGameSettings, but delivers settings from server's to client's version of a character */
	UFUNCTION(server, Reliable)
	void Client_ReportGameSettings(FName InMatchState, float InWarmupDuration, float InMatchDuration, float InLevelStartingTime);

	/** Reports the current time to the client, in response to Server_RequestServerTime */
	UFUNCTION(Client, Reliable)
	void Client_ReportServerTime(float InTimeOfClientRequest, float InServerTime);

	void HandleMatchState();

	UFUNCTION()
	void OnRep_MatchState();

	void DefaultInitHud(AShooterCharacter* ShooterCharacter);

//protected methods
protected:
	//~ Begin AActor Inteface
	virtual void BeginPlay() override;
	//~ End AActor Interface

//public methods
public:

	//~ Begin UObject Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface

	//~ Begin AActor Interface
	virtual void Tick(float DeltaSeconds) override;
	//~ End AActor Interface

	//~ Begin APlayerController Interface
	virtual void OnPossess(APawn* InPawn) override;

	/** Sync with server clock ASAP */
	virtual void ReceivedPlayer() override;
	//~ End APlayerController Interface

	/** Current world time on server, synchronized */
	virtual float GetServerTime();

	void OnMatchStateSet(FName InState);


//public getters/setters
public:
	AShooterHUD* GetPlayerHud();
};
