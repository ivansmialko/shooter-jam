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

void AShooterCharacterController::SetHudHealth(float InHealth, float InMaxHealth)
{
	if (!ShooterHud)
	{
		ShooterHud = Cast<AShooterHUD>(GetHUD());
	}

	if (!ShooterHud)
		return;

	if (!ShooterHud->GetCharacterOverlay())
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
	UE_LOG(LogTemp, Warning, TEXT("Setting score %d"), FMath::FloorToInt(InScore));

	if (!ShooterHud)
	{
		ShooterHud = Cast<AShooterHUD>(GetHUD());
	}

	if (!ShooterHud)
		return;

	if (!ShooterHud->GetCharacterOverlay())
		return;

	if (!ShooterHud->GetCharacterOverlay()->ScoreAmount)
		return;
	
	FString ScoreString{ FString::Printf(TEXT("%d"), FMath::FloorToInt(InScore)) };
	ShooterHud->GetCharacterOverlay()->ScoreAmount->SetText(FText::FromString(ScoreString));
}

void AShooterCharacterController::SetHudDefeats(int InDefeats)
{
	if (!ShooterHud)
	{
		ShooterHud = Cast<AShooterHUD>(GetHUD());
	}

	if (!ShooterHud)
		return;

	if (!ShooterHud->GetCharacterOverlay())
		return;

	if (!ShooterHud->GetCharacterOverlay()->DefeatsAmount)
		return;

	FString DefeatsString{ FString::Printf(TEXT("%d"), InDefeats) };
	ShooterHud->GetCharacterOverlay()->DefeatsAmount->SetText(FText::FromString(DefeatsString));
}

void AShooterCharacterController::BeginPlay()
{
	Super::BeginPlay();

	ShooterHud = Cast<AShooterHUD>(GetHUD());
}
