// Made by smialko


#include "PlayerControllers/ShooterCharacterController.h"

#include "HUD/ShooterHUD.h"
#include "HUD/CharacterOverlay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

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

void AShooterCharacterController::BeginPlay()
{
	Super::BeginPlay();

	ShooterHud = Cast<AShooterHUD>(GetHUD());
}
