// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "ShooterCharacterController.generated.h"

class AShooterHUD;
class AShooterCharacter;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPingTooHigh, bool, isTooHighPing);

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AShooterCharacterController : public APlayerController
{
	GENERATED_BODY()

//public fields
public:
	FOnPingTooHigh OnTooHighPingDelegate;

//private fields
private:

//~ Begin Exposed members
	/** Frequency, controller is requesting servertime with */
	UPROPERTY(EditAnywhere, Category = Time);
	float TimeSyncFrequency{ 5.f };

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* InputActionExit;

//~ End Exposed members

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
	/** Copy of ShooterGameMode's cooldown duration */
	float CooldownDuration{ 0.f };
	/** Copy of ShooterGameMode's time when level started */
	float LevelStartingTime{ 0.f };
	/**  Timer to update warmup/match countdown every second */
	float CountdownTimer{ -1.0f };
	/** CountdownTimer length*/
	float CountdownTimerFrequency{ 1.0f };
	
	/** Timer to poll-check when HUD becomes available */
	float PollInitHudTimer{ -1.0f };
	/** PollInitHudTimer length */
	float PollInitHudTimerFrequency{ 0.1f };

	/** Timer to show Wifi(high ping) image for a limited time */
	float CheckPingTimer{ -1.0f };

	/** How often ping should be checked (20 seconds) */
	UPROPERTY(EditAnywhere)
	float CheckPingTime{ 20.f };

	/** Timer to hide high ping warning after expiring */
	float PingWarningTimer{ -1.f };

	/** Duration Wifi(high ping) image will be showed at */
	UPROPERTY(EditAnywhere)
	float PingWarningDuration{ 5.f };

	/** Limit of acceptable ping */
	UPROPERTY(EditAnywhere)
	float HighPingTreshold{ 50.f };

	/** Time that it takes for request to reach the server. Calculated every few seconds */
	float SingleTripTime{ 0.f };

	//Current match state, synchronized with ShooterGameMode's match state
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UPROPERTY(ReplicatedUsing = OnRep_ShowTeamsBattleWidget)
	bool bShowTeamsBattleWidget{ false };

//private methods
private:
	/** Sets current match time to HUD */
	void UpdateCountdowns();

	/** Checks if it's the time to hide warning timer */
	void UpdatePingWarning(float InDeltaTime);

	/** Initialized hud variables */
	bool CheckInitHud();

	/** Update ping check timer, check an actual ping, show warning */
	void CheckPing(float InDeltaTime);

	/** Requests the current server time, passing in the client's time when the request was sent */
	UFUNCTION(Server, Reliable)
	void Server_RequestServerTime(float InTimeOfClientRequest);

	/**  Request game settings, such as match time, warmup time, current match state.
		Used to deliver settings for server's version of a character */
	UFUNCTION(Server, Reliable)
	void Server_RequestGameSettings();

	/** Reports to server, if local ping is too high */
	UFUNCTION(Server, Reliable)
	void Server_ReportPingStatus(bool bHighPing);

	/**  Same as Server_RequestGameSettings, but delivers settings from server's to client's version of a character */
	UFUNCTION(Client, Reliable)
	void Client_ReportGameSettings(FName InMatchState, float InWarmupDuration, float InMatchDuration, float InCooldownDuration, float InLevelStartingTime);

	/** Reports the current time to the client, in response to Server_RequestServerTime */
	UFUNCTION(Client, Reliable)
	void Client_ReportServerTime(float InTimeOfClientRequest, float InServerTime);

	/** Shows a message in a world chat, on a client machine */
	UFUNCTION(Client, Reliable)
	void Client_WorldChatEliminaion(APlayerState* InAttacker, APlayerState* InAttacked);

	void HandleMatchState(bool bInIsTeamsMatch = false);

	UFUNCTION()
	void OnRep_MatchState();

	UFUNCTION()
	void OnRep_ShowTeamsBattleWidget();

	void OnInputExit(const FInputActionValue& InValue);

	void DefaultInitHud(AShooterCharacter* ShooterCharacter);

	/** Handle changing match state to "WaitingToStart" */
	void HandleWaitingToStart();

	/** Handle changing match state to "InProgress" */
	void HandleInProgress(bool bInIsTeamsMatch = false);

	/** Handle changing match state to "Cooldown" */
	void HandleCooldown();

//protected methods
protected:
//~ Begin AActor Inteface
	virtual void BeginPlay() override;
//~ End AActor Interface

//~ Begin APlayerController Interface
	/** Bind inputs */
	virtual void SetupInputComponent() override;
//~ End APlayerController Interface

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

	/** Handles match state changes */
	void OnMatchStateSet(FName InState, bool bInIsTeamsMatch = false);

	/** If someone is eliminated - this method called, to display in chat */
	void OnEliminationBroadcast(APlayerState* InAttacker, APlayerState* InAttacked);

//public getters/setters
public:
	AShooterHUD* GetPlayerHud();
	FORCEINLINE float GetSingleTripTime() const { return SingleTripTime; }
};
