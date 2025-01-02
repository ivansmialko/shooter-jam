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

	const float CurrSpreadScaled{ CrosshairSpreadMax * HUDPackage.CrosshairSpread };

	FVector2D CurrSpread{ 0.f, 0.f };
	if (HUDPackage.CrosshairsCenter)
	{
		DrawCrosshair(HUDPackage.CrosshairsCenter, VieportCenter, CurrSpread, HUDPackage.CrosshairsColor);
	}

	if (HUDPackage.CrosshairsLeft)
	{
		CurrSpread.X = CurrSpreadScaled * -1.f;
		CurrSpread.Y = 0.f;

		DrawCrosshair(HUDPackage.CrosshairsLeft, VieportCenter, CurrSpread, HUDPackage.CrosshairsColor);
	}

	if (HUDPackage.CrosshairsRight)
	{
		CurrSpread.X = CurrSpreadScaled;
		CurrSpread.Y = 0.f;

		DrawCrosshair(HUDPackage.CrosshairsRight, VieportCenter, CurrSpread, HUDPackage.CrosshairsColor);
	}

	if (HUDPackage.CrosshairsTop)
	{
		CurrSpread.X = 0.f;
		CurrSpread.Y = CurrSpreadScaled * -1.f;

		DrawCrosshair(HUDPackage.CrosshairsTop, VieportCenter, CurrSpread, HUDPackage.CrosshairsColor);
	}

	if (HUDPackage.CrosshairsBottom)
	{
		CurrSpread.X = 0.f;
		CurrSpread.Y = CurrSpreadScaled;

		DrawCrosshair(HUDPackage.CrosshairsBottom, VieportCenter, CurrSpread, HUDPackage.CrosshairsColor);
	}
}

void AShooterHUD::DrawCrosshair(UTexture2D* InCrosshair, const FVector2D InViewportCenter, const FVector2D Spread, const FLinearColor& CrosshairsColor)
{
	if (!InCrosshair)
		return;

	const float TextureW{ static_cast<float>(InCrosshair->GetSizeX()) };
	const float TextureH{ static_cast<float>(InCrosshair->GetSizeY()) };

	const FVector2D TextureDrawPoint
	{
		InViewportCenter.X - (TextureW * 0.5f) + Spread.X,
		InViewportCenter.Y - (TextureH * 0.5f) + Spread.Y
	};

	DrawTexture(InCrosshair, TextureDrawPoint.X, TextureDrawPoint.Y, TextureW, TextureH, 0.f, 0.f, 1.f, 1.f, CrosshairsColor);
}

void AShooterHUD::SetHUDPackage(FHUDPackage& InHUDPackage)
{
	HUDPackage = InHUDPackage;
}
