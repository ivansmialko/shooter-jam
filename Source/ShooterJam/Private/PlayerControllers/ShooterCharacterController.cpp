// Made by smialko


#include "PlayerControllers/ShooterCharacterController.h"

#include "HUD/ShooterHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Characters/ShooterCharacter.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
void AShooterCharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(InPawn);
	if (!ShooterCharacter)
		return;

	SetHudHealth(ShooterCharacter->GetHealth(), ShooterCharacter->GetMaxHealth());
}

void AShooterCharacterController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
	}
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

void AShooterCharacterController::SetHudHealth(float InHealth, float InMaxHealth)
{
	if (!CheckInitHud())
		return;

	if(!ShooterHud->GetCharacterOverlay()->HealthBar
		|| !ShooterHud->GetCharacterOverlay()->HealthText)
		return;

	const float HealthPercent{ InHealth / InMaxHealth };
	ShooterHud->GetCharacterOverlay()->HealthBar->SetPercent(HealthPercent);

	FString HealthString{ FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(InHealth), FMath::CeilToInt(InMaxHealth)) };
	ShooterHud->GetCharacterOverlay()->HealthText->SetText(FText::FromString(HealthString));
}

void AShooterCharacterController::SetHudScore(float InScore)
{
	if (!CheckInitHud())
		return;

	if (!ShooterHud->GetCharacterOverlay()->ScoreAmount)
		return;
	
	FString ScoreString{ FString::Printf(TEXT("%d"), FMath::FloorToInt(InScore)) };
	ShooterHud->GetCharacterOverlay()->ScoreAmount->SetText(FText::FromString(ScoreString));
}

void AShooterCharacterController::SetHudDefeats(int32 InDefeats)
{
	if (!CheckInitHud())
		return;

	if (!ShooterHud->GetCharacterOverlay()->DefeatsAmount)
		return;

	FString DefeatsString{ FString::Printf(TEXT("%d"), InDefeats) };
	ShooterHud->GetCharacterOverlay()->DefeatsAmount->SetText(FText::FromString(DefeatsString));
}

void AShooterCharacterController::SetHudWeaponAmmo(int32 InAmmo)
{
	if (!CheckInitHud())
		return;

	if (!ShooterHud->GetCharacterOverlay()->WeaponAmmoAmount)
		return;

	FString WeaponAmmoString{ FString::Printf(TEXT("%d"), InAmmo) };
	ShooterHud->GetCharacterOverlay()->WeaponAmmoAmount->SetText(FText::FromString(WeaponAmmoString));
}

void AShooterCharacterController::SetHudWeaponAmmoEmpty()
{
	if (!CheckInitHud())
		return;

	if (!ShooterHud->GetCharacterOverlay()->WeaponAmmoAmount)
		return;

	ShooterHud->GetCharacterOverlay()->WeaponAmmoAmount->SetText(FText::FromString(TEXT("*")));
}

void AShooterCharacterController::SetHudMatchCountdown(float InCountdownTime)
{
	if (!CheckInitHud())
		return;

	if (!ShooterHud->GetCharacterOverlay()->MatchCountdownText)
		return;

	int32 Minutes{ FMath::FloorToInt(InCountdownTime / 60) };
	int32 Seconds{ static_cast<int32>(InCountdownTime) - Minutes * 60 };

	FString CountdownString{ FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds) };
	ShooterHud->GetCharacterOverlay()->MatchCountdownText->SetText(FText::FromString(CountdownString));
}

void AShooterCharacterController::SetHudCarriedAmmo(int32 InAmmo)
{
	if (!CheckInitHud())
		return;

	if (!ShooterHud->GetCharacterOverlay()->CarriedAmmoAmount)
		return;

	FString CarriedAmmoString{ FString::Printf(TEXT("%d"), InAmmo) };
	ShooterHud->GetCharacterOverlay()->CarriedAmmoAmount->SetText(FText::FromString(CarriedAmmoString));
}

void AShooterCharacterController::SetHudCarriedAmmoEmpty()
{
	if (!CheckInitHud())
		return;

	if (!ShooterHud->GetCharacterOverlay()->CarriedAmmoAmount)
		return;

	ShooterHud->GetCharacterOverlay()->CarriedAmmoAmount->SetText(FText::FromString(TEXT("*")));
}

bool AShooterCharacterController::CheckInitHud()
{
	if (!ShooterHud)
	{
		ShooterHud = Cast<AShooterHUD>(GetHUD());
	}

	if (!ShooterHud)
		return false;

	if (!ShooterHud->GetCharacterOverlay())
		return false;

	return true;
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

void AShooterCharacterController::BeginPlay()
{
	Super::BeginPlay();

	ShooterHud = Cast<AShooterHUD>(GetHUD());
}

void AShooterCharacterController::SetHudTime()
{
	int64 SecondsLeft{ FMath::CeilToInt(MatchTime - GetServerTime()) };
	UE_LOG(LogTemp, Warning, TEXT("%f"), GetServerTime());
	if (SecondsLeft != MatchTimeLeft)
	{
		SetHudMatchCountdown(static_cast<float>(SecondsLeft));
	}
	MatchTimeLeft = SecondsLeft;
}
