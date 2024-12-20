// Made by smialko


#include "HUD/ShooterHUD.h"

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!GEngine)
		return;

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	const FVector2D VieportCenter{ ViewportSize.X * 0.5f, ViewportSize.Y * 0.5f };

	if (HUDPackage.CrosshairsCenter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Drawing center"));
		DrawCrosshair(HUDPackage.CrosshairsCenter, VieportCenter);
	}

	if (HUDPackage.CrosshairsLeft)
	{
		DrawCrosshair(HUDPackage.CrosshairsLeft, VieportCenter);
	}

	if (HUDPackage.CrosshairsRight)
	{
		DrawCrosshair(HUDPackage.CrosshairsRight, VieportCenter);
	}

	if (HUDPackage.CrosshairsTop)
	{
		DrawCrosshair(HUDPackage.CrosshairsTop, VieportCenter);
	}

	if (HUDPackage.CrosshairsBottom)
	{
		DrawCrosshair(HUDPackage.CrosshairsBottom, VieportCenter);
	}
}

void AShooterHUD::DrawCrosshair(UTexture2D* InCrosshair, const FVector2D InViewportCenter)
{
	if (!InCrosshair)
		return;

	const float TextureW{ static_cast<float>(InCrosshair->GetSizeX()) };
	const float TextureH{ static_cast<float>(InCrosshair->GetSizeY()) };

	const FVector2D TextureDrawPoint{ InViewportCenter.X - (TextureW * 0.5f), InViewportCenter.Y - (TextureH * 0.5f) };

	DrawTexture(InCrosshair, TextureDrawPoint.X, TextureDrawPoint.Y, TextureW, TextureH, 0.f, 0.f, 1.f, 1.f, FLinearColor::White);
}

void AShooterHUD::SetHUDPackage(FHUDPackage& InHUDPackage)
{
	HUDPackage = InHUDPackage;
}
