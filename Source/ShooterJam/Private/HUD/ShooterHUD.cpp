// Made by smialko


#include "HUD/ShooterHUD.h"
#include "HUD/CharacterOverlay.h"

#include "GameFramework/PlayerController.h"

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!GEngine)
		return;

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	const FVector2D VieportCenter{ ViewportSize.X * 0.5f, ViewportSize.Y * 0.5f };

	const float CurrSpreadScaled{ CrosshairSpreadMax * HudPackage.CrosshairSpread };

	FVector2D CurrSpread{ 0.f, 0.f };
	if (HudPackage.CrosshairsCenter)
	{
		DrawCrosshair(HudPackage.CrosshairsCenter, VieportCenter, CurrSpread, HudPackage.CrosshairsColor);
	}

	if (HudPackage.CrosshairsLeft)
	{
		CurrSpread.X = CurrSpreadScaled * -1.f;
		CurrSpread.Y = 0.f;

		DrawCrosshair(HudPackage.CrosshairsLeft, VieportCenter, CurrSpread, HudPackage.CrosshairsColor);
	}

	if (HudPackage.CrosshairsRight)
	{
		CurrSpread.X = CurrSpreadScaled;
		CurrSpread.Y = 0.f;

		DrawCrosshair(HudPackage.CrosshairsRight, VieportCenter, CurrSpread, HudPackage.CrosshairsColor);
	}

	if (HudPackage.CrosshairsTop)
	{
		CurrSpread.X = 0.f;
		CurrSpread.Y = CurrSpreadScaled * -1.f;

		DrawCrosshair(HudPackage.CrosshairsTop, VieportCenter, CurrSpread, HudPackage.CrosshairsColor);
	}

	if (HudPackage.CrosshairsBottom)
	{
		CurrSpread.X = 0.f;
		CurrSpread.Y = CurrSpreadScaled;

		DrawCrosshair(HudPackage.CrosshairsBottom, VieportCenter, CurrSpread, HudPackage.CrosshairsColor);
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

void AShooterHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (!PlayerController)
		return;

	if (!CharacterOverlayClass)
		return;

	CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
	if (!CharacterOverlay)
		return;

	CharacterOverlay->AddToViewport();
}

void AShooterHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AShooterHUD::SetHudPackage(FHUDPackage& InHUDPackage)
{
	HudPackage = InHUDPackage;
}
