// Made by smialko


#include "PlayerControllers/ShooterCharacterController.h"

#include "HUD/ShooterHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Characters/ShooterCharacter.h"
#include "GameModes/ShooterGameMode.h"
#include "PlayerState/ShooterPlayerState.h"

#include "Net/UnrealNetwork.h"

void AShooterCharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(InPawn);
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

	SetHudTime();
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

void AShooterCharacterController::Client_ReportServerTime_Implementation(float InTimeOfClientRequest, float InServerTime)
{
	float RoundTripTime{ static_cast<float>(GetWorld()->GetTimeSeconds()) - InTimeOfClientRequest };
	float CurrentServerTime{ InServerTime + RoundTripTime * 0.5f };

	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void AShooterCharacterController::HandleMatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		if (!CheckInitHud())
			return;

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
}

void AShooterCharacterController::BeginPlay()
{
	Super::BeginPlay();

	ShooterHud = Cast<AShooterHUD>(GetHUD());
}

void AShooterCharacterController::SetHudTime()
{
	if (!GetPlayerHud())
		return;

	int64 SecondsLeft{ FMath::CeilToInt(MatchTime - GetServerTime()) };
	UE_LOG(LogTemp, Warning, TEXT("%f"), GetServerTime());
	if (SecondsLeft != MatchTimeLeft)
	{
		GetPlayerHud()->SetMatchCountdown(static_cast<float>(SecondsLeft));
	}
	MatchTimeLeft = SecondsLeft;
}
