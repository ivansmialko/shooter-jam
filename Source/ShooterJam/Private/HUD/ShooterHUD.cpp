// Made by smialko


#include "HUD/ShooterHUD.h"
#include "HUD/CharacterOverlay.h"
#include "HUD/AnnouncementWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!GEngine)
		return;

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	const FVector2D VieportCenter{ ViewportSize.X * 0.5f, ViewportSize.Y * 0.5f };

	const float CurrSpreadScaled{ CrosshairSpreadMax * CrosshairsPackage.CrosshairSpread };

	FVector2D CurrSpread{ 0.f, 0.f };
	if (CrosshairsPackage.CrosshairsCenter)
	{
		DrawCrosshair(CrosshairsPackage.CrosshairsCenter, VieportCenter, CurrSpread, CrosshairsPackage.CrosshairsColor);
	}

	if (CrosshairsPackage.CrosshairsLeft)
	{
		CurrSpread.X = CurrSpreadScaled * -1.f;
		CurrSpread.Y = 0.f;

		DrawCrosshair(CrosshairsPackage.CrosshairsLeft, VieportCenter, CurrSpread, CrosshairsPackage.CrosshairsColor);
	}

	if (CrosshairsPackage.CrosshairsRight)
	{
		CurrSpread.X = CurrSpreadScaled;
		CurrSpread.Y = 0.f;

		DrawCrosshair(CrosshairsPackage.CrosshairsRight, VieportCenter, CurrSpread, CrosshairsPackage.CrosshairsColor);
	}

	if (CrosshairsPackage.CrosshairsTop)
	{
		CurrSpread.X = 0.f;
		CurrSpread.Y = CurrSpreadScaled * -1.f;

		DrawCrosshair(CrosshairsPackage.CrosshairsTop, VieportCenter, CurrSpread, CrosshairsPackage.CrosshairsColor);
	}

	if (CrosshairsPackage.CrosshairsBottom)
	{
		CurrSpread.X = 0.f;
		CurrSpread.Y = CurrSpreadScaled;

		DrawCrosshair(CrosshairsPackage.CrosshairsBottom, VieportCenter, CurrSpread, CrosshairsPackage.CrosshairsColor);
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
	if (CharacterOverlay && CharacterOverlay->IsInViewport())
	{
		CharacterOverlay->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	APlayerController* PlayerController = GetOwningPlayerController();
	if (!PlayerController)
		return;

	if (!CharacterOverlayBlueprint)
		return;

	CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayBlueprint);
	if (!CharacterOverlay)
		return;

	CharacterOverlay->AddToViewport();
}

void AShooterHUD::AddAnnouncementWidget()
{
	if (AnnouncementWidget && AnnouncementWidget->IsInViewport())
	{
		AnnouncementWidget->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	APlayerController* PlayerController = GetOwningPlayerController();
	if (!PlayerController)
		return;

	if (!AnnouncementWidgetBlueprint)
		return;

	AnnouncementWidget = CreateWidget<UAnnouncementWidget>(PlayerController, AnnouncementWidgetBlueprint);
	if (!AnnouncementWidget)
		return;

	AnnouncementWidget->AddToViewport();
}

void AShooterHUD::HideAnnouncementWidget()
{
	if (!AnnouncementWidget)
		return;

	AnnouncementWidget->SetVisibility(ESlateVisibility::Hidden);
}

void AShooterHUD::HideCharacterOverlay()
{
	if (!CharacterOverlay)
		return;

	CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);
}

void AShooterHUD::HideAnnouncementInfoText()
{
	if (!AnnouncementWidget)
		return;

	if (!AnnouncementWidget->InfoText)
		return;

	AnnouncementWidget->InfoText->SetVisibility(ESlateVisibility::Hidden);
}

void AShooterHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AShooterHUD::SetCrosshairsPackage(FCrosshairsPackage& InCrosshairsPackage)
{
	CrosshairsPackage = InCrosshairsPackage;
}

void AShooterHUD::SetHealth(float InHealth, float InMaxHealth)
{
	//Cache values
	Health = InHealth;
	MaxHealth = InMaxHealth;

	if (!CharacterOverlay)
		return;

	if (!CharacterOverlay->HealthText ||
		!CharacterOverlay->HealthBar)
		return;

	const float HealthPercent{ Health / MaxHealth };
	CharacterOverlay->HealthBar->SetPercent(HealthPercent);

	FString HealthString{ FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(InHealth), FMath::CeilToInt(InMaxHealth)) };
	CharacterOverlay->HealthText->SetText(FText::FromString(HealthString));
}

void AShooterHUD::SetShield(float InShield, float InMaxShield)
{
	if (!CharacterOverlay)
		return;

	if (!CharacterOverlay->ShieldText ||
		!CharacterOverlay->ShieldBar)
		return;

	CharacterOverlay->ShieldBar->SetVisibility(InShield > 0.f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	CharacterOverlay->ShieldText->SetVisibility(InShield > 0.f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	const float ShieldPercent{ InShield / InMaxShield };
	CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);

	FString ShieldString{ FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(InShield), FMath::CeilToInt(InMaxShield)) };
	CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldString));
}

void AShooterHUD::SetScore(float InScore)
{
	//Cache value
	Score = InScore;

	if (!CharacterOverlay)
		return;

	if (!CharacterOverlay->ScoreAmount)
		return;

	FString ScoreString{ FString::Printf(TEXT("%d"), FMath::FloorToInt(InScore)) };
	CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreString));
}

void AShooterHUD::SetDefeats(int32 InDefeats)
{
	//Cache value
	Defeats = InDefeats;

	if (!CharacterOverlay)
		return;

	if (!CharacterOverlay->DefeatsAmount)
		return;

	FString DefeatsString{ FString::Printf(TEXT("%d"), InDefeats) };
	CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsString));
}

void AShooterHUD::SetWeaponAmmo(int32 InAmmo)
{
	//Cache value
	Ammo = InAmmo;

	if (!CharacterOverlay)
		return;

	if (!CharacterOverlay->WeaponAmmoAmount)
		return;

	FString WeaponAmmoString{ FString::Printf(TEXT("%d"), InAmmo) };
	CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(WeaponAmmoString));
}

void AShooterHUD::SetCarriedAmmoEmpty()
{
	if (!CharacterOverlay)
		return;

	if (!CharacterOverlay->CarriedAmmoAmount)
		return;

	CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(TEXT("*")));
}

void AShooterHUD::SetCarriedAmmo(int32 InAmmo)
{
	//Cache value
	CarriedAmmo = InAmmo;

	if (!CharacterOverlay)
		return;

	if (!CharacterOverlay->CarriedAmmoAmount)
		return;

	FString CarriedAmmoString{ FString::Printf(TEXT("%d"), InAmmo) };
	CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(CarriedAmmoString));
}

void AShooterHUD::SetWeaponAmmoEmpty()
{
	if (!CharacterOverlay)
		return;

	if (!CharacterOverlay->WeaponAmmoAmount)
		return;

	CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(TEXT("*")));
}

void AShooterHUD::SetMatchCountdown(float InCountdownTime)
{
	if (!CharacterOverlay)
		return;

	if (!CharacterOverlay->MatchCountdownText)
		return;

	if (InCountdownTime <= 0.f)
	{
		CharacterOverlay->MatchCountdownText->SetText(FText());
	}

	int32 Minutes{ FMath::FloorToInt(InCountdownTime / 60) };
	int32 Seconds{ static_cast<int32>(InCountdownTime) - Minutes * 60 };
	
	FString CountdownString{ FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds) };
	CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownString));
}

void AShooterHUD::SetWarmupCountdown(float InCountDownTime)
{
	if (!AnnouncementWidget)
		return;

	if (!AnnouncementWidget->WarmupTime)
		return;

	if (InCountDownTime < 0.f)
	{
		AnnouncementWidget->WarmupTime->SetText(FText());
		return;
	}

	int32 Minutes{ FMath::FloorToInt(InCountDownTime / 60) };
	int32 Seconds{ static_cast<int32>(InCountDownTime) - Minutes * 60 };
	
	FString CountdownString{ FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds) };
	AnnouncementWidget->WarmupTime->SetText(FText::FromString(CountdownString));
}

void AShooterHUD::SetAnnouncementText(FText InText)
{
	if (!AnnouncementWidget)
		return;

	if (!AnnouncementWidget->AnnouncementText)
		return;

	AnnouncementWidget->AnnouncementText->SetText(InText);
}

void AShooterHUD::SetAnnouncementInfoText(FText InText)
{
	if (!AnnouncementWidget)
		return;

	if (!AnnouncementWidget->InfoText)
		return;

	AnnouncementWidget->InfoText->SetText(InText);
}


void AShooterHUD::SetGrenadesAmount(int32 InGrenades)
{
	if (!CharacterOverlay)
		return;

	if (!CharacterOverlay->GrenadesAmountText)
		return;

	FString GrenadesAmountText{ FString::Printf(TEXT("%d"), InGrenades) };
	CharacterOverlay->GrenadesAmountText->SetText(FText::FromString(GrenadesAmountText));
}
