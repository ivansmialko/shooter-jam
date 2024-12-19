// Made by smialko


#include "HUD/ShooterHUD.h"

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AShooterHUD::SetHUDPackage(FHUDPackage& InHUDPackage)
{
	HUDPackage = InHUDPackage;
}
