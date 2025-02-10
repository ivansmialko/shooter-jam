// Made by smialko


#include "PlayerControllers/ShooterCharacterController.h"

#include "HUD/ShooterHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Characters/ShooterCharacter.h"
#include "GameModes/ShooterGameMode.h"
#include "PlayerState/ShooterPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void AShooterCharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(InPawn);
	if (!ShooterCharacter)
		return;

	DefaultInitHud(ShooterCharacter);

	ShooterCharacter->InitInputs();
}

void AShooterCharacterController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
	}
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
				Server_RequestGameSettings_Implementation();
				CountdownTimer = CountdownTimerFrequency;
			}
			else
			{
				PollInitHudTimer = PollInitHudTimerFrequency;
			}
		}
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

bool AShooterCharacterController::CheckInitHud()
{
	if (!ShooterHud)
	{
		ShooterHud = Cast<AShooterHUD>(GetHUD());
	}

	return ShooterHud != nullptr;
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
	LevelStartingTime = GameMode->GetLevelStartingTime();
	MatchState = GameMode->GetMatchState();

	Client_ReportGameSettings_Implementation(MatchState, WarmupDuration, MatchDuration, LevelStartingTime);
}

void AShooterCharacterController::Client_ReportGameSettings_Implementation(FName InMatchState, float InWarmupDuration, float InMatchDuration, float InLevelStartingTime)
{
	MatchState = InMatchState;
	WarmupDuration = InWarmupDuration;
	MatchDuration = InMatchDuration;
	LevelStartingTime = InLevelStartingTime;
	OnMatchStateSet(MatchState);
}

void AShooterCharacterController::Client_ReportServerTime_Implementation(float InTimeOfClientRequest, float InServerTime)
{
	float RoundTripTime{ static_cast<float>(GetWorld()->GetTimeSeconds()) - InTimeOfClientRequest };
	float CurrentServerTime{ InServerTime + RoundTripTime * 0.5f };

	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void AShooterCharacterController::HandleMatchState()
{
	if (MatchState == MatchState::WaitingToStart)
	{
		if (!CheckInitHud())
			return;

		ShooterHud->AddAnnouncementWidget();
		UpdateCountdowns();
	}

	if (MatchState == MatchState::InProgress)
	{
		if (!CheckInitHud())
			return;

		ShooterHud->HideAnnouncementWidget();
		ShooterHud->AddCharacterOverlay();
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

void AShooterCharacterController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		PollInitHudTimer = PollInitHudTimerFrequency;
	}
}

void AShooterCharacterController::UpdateCountdowns()
{
	if (!GetPlayerHud())
		return;

	if (MatchState == MatchState::WaitingToStart)
	{
		float TimeLeft = WarmupDuration - GetServerTime() + LevelStartingTime;
		GetPlayerHud()->SetWarmupCountdown(static_cast<float>(TimeLeft));
	}
	else if (MatchState == MatchState::InProgress)
	{
		float TimeLeft = WarmupDuration + MatchDuration - GetServerTime() + LevelStartingTime;
		GetPlayerHud()->SetMatchCountdown(static_cast<float>(TimeLeft));
	}
}
