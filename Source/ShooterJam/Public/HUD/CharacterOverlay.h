// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

class UProgressBar;
class UTextBlock;
class UStackBox;
class UImage;
class UWorldChat;

/**
 * C++ base for W_CharacterOverlay. Collection of widgets that represents user's HUD
 */
UCLASS()
class SHOOTERJAM_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreAmount;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefeatsAmount;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponAmmoAmount;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchCountdownText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GrenadesAmountText;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ShieldBar;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ShieldText;
	UPROPERTY(meta = (BindWidget))
	UImage* WifiImage;
	UPROPERTY(meta = (BindWidget))
	UWorldChat* WorldChat;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* WifiBlinkAnimation;
};