// Made by smialko


#include "PlayerControllers/ShooterCharacterController.h"

#include "HUD/ShooterHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Characters/ShooterCharacter.h"
#include "GameModes/ShooterGameMode.h"
#include "PlayerState/ShooterPlayerState.h"
#include "GameStates/ShooterGameState.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

bool AShooterCharacterController::CheckInitHud()
{
	if (!ShooterHud)
	{
		ShooterHud = Cast<AShooterHUD>(GetHUD());
	}

	return ShooterHud != nullptr;
}

void AShooterCharacterController::CheckPing(float InDeltaTime)
{
	if (CheckPingTimer <= 0.f)
		return;

	CheckPingTimer -= InDeltaTime;

	if (CheckPingTimer > 0.f)
		return;

	if (!PlayerState)
	{
		PlayerState = GetPlayerState<APlayerState>();
		if (!PlayerState)
			return;
	}

	if (PlayerState->GetPingInMilliseconds() < HighPingTreshold)
	{
		CheckPingTimer = CheckPingTime;
		return;
	}

	if (!GetPlayerHud())
		return;

	GetPlayerHud()->ShowPingAnimation();

	PingWarningTimer = PingWarningDuration;
	CheckPingTimer = CheckPingTime;
}

void AShooterCharacterController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterCharacterController, MatchState);
}

void AShooterCharacterController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocalController())
	{
		TimeSyncTimer -= DeltaSeconds;
		if (TimeSyncTimer <= 0.f)
		{
			Server_RequestServerTime(GetWorld()->GetTimeSeconds());
			TimeSyncTimer = TimeSyncFrequency;
		}
	}

	if (CountdownTimer > 0.f)
	{
		CountdownTimer -= DeltaSeconds;
		if (CountdownTimer <= 0.f)
		{
			UpdateCountdowns();
			CountdownTimer = CountdownTimerFrequency;
		}
	}

	if (PollInitHudTimer > 0.f)
	{
		PollInitHudTimer -= DeltaSeconds;
		if (PollInitHudTimer <= 0.f)
		{
			if (CheckInitHud())
			{
				HandleMatchState();
			}
			else
			{
				PollInitHudTimer = PollInitHudTimerFrequency;
			}
		}
	}

	CheckPing(DeltaSeconds);
	UpdatePingWarning(DeltaSeconds);
}

void AShooterCharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(InPawn);
	if (!ShooterCharacter)
		return;

	DefaultInitHud(ShooterCharacter);
}

void AShooterCharacterController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

float AShooterCharacterController::GetServerTime()
{
	if (HasAuthority())
		return GetWorld()->GetTimeSeconds();

	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AShooterCharacterController::OnMatchStateSet(FName InState)
{
	MatchState = InState;
	HandleMatchState();
}

AShooterHUD* AShooterCharacterController::GetPlayerHud()
{
	if (!CheckInitHud())
		return nullptr;

	return ShooterHud;
}

void AShooterCharacterController::Server_RequestServerTime_Implementation(float InTimeOfClientRequest)
{
	float ServerTime{ static_cast<float>(GetWorld()->GetTimeSeconds()) };
	Client_ReportServerTime(InTimeOfClientRequest, ServerTime);
}

void AShooterCharacterController::Server_RequestGameSettings_Implementation()
{
	AShooterGameMode* GameMode = Cast<AShooterGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode)
		return;

	WarmupDuration = GameMode->GetWarmupDuration();
	MatchDuration = GameMode->GetMatchDuration();
	CooldownDuration = GameMode->GetCooldownDuration();
	LevelStartingTime = GameMode->GetLevelStartingTime();
	MatchState = GameMode->GetMatchState();

	Client_ReportGameSettings(MatchState, WarmupDuration, MatchDuration, CooldownDuration, LevelStartingTime);
}

void AShooterCharacterController::Client_ReportGameSettings_Implementation(FName InMatchState, float InWarmupDuration, float InMatchDuration, float InCooldownDuration, float InLevelStartingTime)
{
	MatchState = InMatchState;
	WarmupDuration = InWarmupDuration;
	MatchDuration = InMatchDuration;
	CooldownDuration = InCooldownDuration;
	LevelStartingTime = InLevelStartingTime;
	OnMatchStateSet(MatchState);
}

void AShooterCharacterController::Client_ReportServerTime_Implementation(float InTimeOfClientRequest, float InServerTime)
{
	float RoundTripTime{ static_cast<float>(GetWorld()->GetTimeSeconds()) - InTimeOfClientRequest };
	float CurrentServerTime{ InServerTime + RoundTripTime * 0.5f };

	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void AShooterCharacterController::UpdateCountdowns()
{
	if (!GetPlayerHud())
		return;

	if (MatchState == MatchState::WaitingToStart)
	{
		float TimeLeft = WarmupDuration - GetServerTime() + LevelStartingTime;
		GetPlayerHud()->SetWarmupCountdown(TimeLeft);
	}
	else if (MatchState == MatchState::InProgress)
	{
		float TimeLeft = WarmupDuration + MatchDuration - GetServerTime() + LevelStartingTime;
		GetPlayerHud()->SetMatchCountdown(TimeLeft);
	}
	else if (MatchState == MatchState::Cooldown)
	{
		float TimeLeft = CooldownDuration + WarmupDuration + MatchDuration - GetServerTime() + LevelStartingTime + CountdownTimer;
		GetPlayerHud()->SetWarmupCountdown(TimeLeft);
	}
}

void AShooterCharacterController::UpdatePingWarning(float InDeltaTime)
{
	if (PingWarningTimer <= 0.f)
		return;

	PingWarningTimer -= InDeltaTime;

	if (PingWarningTimer > 0.f)
		return;

	if (!GetPlayerHud())
		return;

	GetPlayerHud()->HidePingAnimation();

	PingWarningTimer = -1.0f;
}

void AShooterCharacterController::HandleMatchState()
{
	if (MatchState == MatchState::WaitingToStart)
	{
		HandleWaitingToStart();
	}
	if (MatchState == MatchState::InProgress)
	{
		HandleInProgress();
	}
	if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AShooterCharacterController::OnRep_MatchState()
{
	HandleMatchState();
}

void AShooterCharacterController::DefaultInitHud(AShooterCharacter* InShooterCharacter)
{
	if (!InShooterCharacter)
		return;

	if (!GetPlayerHud())
		return;

	GetPlayerHud()->SetHealth(InShooterCharacter->GetHealth(), InShooterCharacter->GetMaxHealth());
	GetPlayerHud()->SetWeaponAmmoEmpty();
	GetPlayerHud()->SetCarriedAmmoEmpty();

	AShooterPlayerState* ShooterPlayerState = GetPlayerState<AShooterPlayerState>();
	if (!ShooterPlayerState)
		return;

	GetPlayerHud()->SetScore(ShooterPlayerState->GetScore());
	GetPlayerHud()->SetDefeats(ShooterPlayerState->GetDefeats());
	UpdateCountdowns();
}

void AShooterCharacterController::HandleWaitingToStart()
{
	if (!CheckInitHud())
		return;

	CountdownTimer = CountdownTimerFrequency;
	ShooterHud->AddAnnouncementWidget();
	UpdateCountdowns();
}

void AShooterCharacterController::HandleInProgress()
{
	if (!CheckInitHud())
		return;

	ShooterHud->HideAnnouncementWidget();
	ShooterHud->AddCharacterOverlay();
	UpdateCountdowns();
}

void AShooterCharacterController::HandleCooldown()
{
	if (CheckInitHud())
	{
		ShooterHud->HideCharacterOverlay();

		ShooterHud->AddAnnouncementWidget();
		ShooterHud->SetAnnouncementText(FText::FromString(FString(TEXT("Waiting for the next match to start.."))));
		UpdateCountdowns();

		AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
		AShooterPlayerState* ShooterPlayerState = GetPlayerState<AShooterPlayerState>();
		if (ShooterGameState && ShooterPlayerState)
		{
			const TArray<AShooterPlayerState*>& TopScoringPlayers{ ShooterGameState->GetTopScoringPlayers() };
			FString InfoTextString;
			if (TopScoringPlayers.IsEmpty())
			{
				InfoTextString = FString("There is no winner");
			}
			else if(TopScoringPlayers[0] == ShooterPlayerState)
			{
				InfoTextString = FString("You are the winner!");
			}
			else
			{
				InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopScoringPlayers[0]->GetPlayerName());
			}

			ShooterHud->SetAnnouncementInfoText(FText::FromString(InfoTextString));
		}
	}

	if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn()))
	{
		ShooterCharacter->DisableGameplay();
	}
}

void AShooterCharacterController::BeginPlay()
{
	Super::BeginPlay();

	PollInitHudTimer = PollInitHudTimerFrequency;
	CheckPingTimer = CheckPingTime;

	Server_RequestGameSettings();

	//FString LocalRoleString = UEnum::GetDisplayValueAsText<ENetRole>(GetLocalRole()).ToString();
	//FString SysMessage = FString::Printf(TEXT("Player %s, %d, is local: %s."), *LocalRoleString, static_cast<int>(GetNetMode()), (IsLocalController() ? TEXT("true") : TEXT("false")));
	//FString UserMessage = FString::Printf(TEXT("BeginPlay(), i'm %s."), *GetName());
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("%s%s"), *SysMessage, *UserMessage));
}

