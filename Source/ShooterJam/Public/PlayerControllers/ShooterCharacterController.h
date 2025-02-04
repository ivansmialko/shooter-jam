// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "ShooterCharacterController.generated.h"

class AShooterHUD;

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

	float MatchTime{ 120.f };
	int32 MatchTimeLeft{ 0 };
	
	//Current match state, syncronized with ShooterGameMode's match state
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

//private methods
private:

	/** Sets current match time to HUD */
	void SetHudTime();

	/** Initialized hud variables */
	bool CheckInitHud();

	/** Requests the current server time, passing in the client's time when the request was sent */
	UFUNCTION(Server, Reliable)
	void Server_RequestServerTime(float InTimeOfClientRequest);

	/** Reports the current time to the client, in response to Server_RequestServerTime */
	UFUNCTION(Client, Reliable)
	void Client_ReportServerTime(float InTimeOfClientRequest, float InServerTime);

	void OnRep_MatchState();

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

	void SetHudHealth(float InHealth, float InMaxHealth);
	void SetHudScore(float InScore);
	void SetHudDefeats(int32 InDefeats);
	void SetHudWeaponAmmo(int32 InAmmo);
	void SetHudCarriedAmmoEmpty();
	void SetHudCarriedAmmo(int32 InAmmo);
	void SetHudWeaponAmmoEmpty();
	void SetHudMatchCountdown(float InCountdownTime);
};
