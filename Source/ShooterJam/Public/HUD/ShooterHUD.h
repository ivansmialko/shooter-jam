// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

USTRUCT(BlueprintType)
struct FCrosshairsPackage 
{
	GENERATED_BODY()

public:
	class UTexture2D* CrosshairsCenter{ nullptr };
	UTexture2D* CrosshairsLeft{ nullptr };
	UTexture2D* CrosshairsRight{ nullptr };
	UTexture2D* CrosshairsTop{ nullptr };
	UTexture2D* CrosshairsBottom{ nullptr };
	FLinearColor CrosshairsColor;
	float CrosshairSpread{ 0.f };
};

class UCharacterOverlay;
class UAnnouncementWidget;

UCLASS()
class SHOOTERJAM_API AShooterHUD : public AHUD
{
	GENERATED_BODY()
	
//private variables
private:
	//** Package for users crosshair images
	FCrosshairsPackage CrosshairsPackage;

	//** Class of user's actual HUD widget
	UCharacterOverlay* CharacterOverlay{ nullptr };

	//** Class of announcement widget
	UAnnouncementWidget* AnnouncementWidget{ nullptr };

	//** Blueprint selector, to set a derived from UCharacterOverlay widget
	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> CharacterOverlayBlueprint;

	//** Blueprint selector, to set a derived from UAnnouncementWidget widget
	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> AnnouncementWidgetBlueprint;

	//** Max crosshair spread
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax{ 60.f };

	//** Used to cache Health value
	float Health{ 0.f };
	//** Used to cache Maximum Health value
	float MaxHealth{ 0.f };
	//** Used to cache match Countdown time
	float CountdownTime{ 0.f };
	//** Used to cache users Score value
	float Score{ 0.f };
	//** User to cache defeats amount
	int32 Defeats{ 0 };
	//** Used to cache current Ammo amount
	int32 Ammo{ 0 };
	//** Used to cache Carried Ammo amount
	int32 CarriedAmmo{ 0 };

//private methods
private:
	void DrawCrosshair(UTexture2D* InCrosshair, const FVector2D InViewportCenter, const FVector2D Spread, const FLinearColor& CrosshairsColor);

//protected methods
protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

//public methods
public:
	//~ Begin AHUD Interface
	virtual void DrawHUD() override;
	//~ End AHUD Interface

	/** Creates an instance of UUserWidget and add it to the screen */ 
	void AddCharacterOverlay();

	/** Creates an instance of UUserWidget and add it to the screen */
	void AddAnnouncementWidget();

	/** Hides an announcement widget */
	void HideAnnouncementWidget();

	/** Hides a character overlay widget */
	void HideCharacterOverlay();

	/** Hides an UAnnouncementWidget::InfoText */
	void HideAnnouncementInfoText();

	/** Shows a ping animation (blinking wifi icon) */
	void ShowPingAnimation();

	/** Hides a ping animation */
	void HidePingAnimation();


//public getters/setters
public:
	FORCEINLINE bool IsOverlayInitialized() const { return CharacterOverlay != nullptr; }
	FORCEINLINE UCharacterOverlay* GetCharacterOverlay() const { return CharacterOverlay; }

	void SetCrosshairsPackage(FCrosshairsPackage& InHUDPackage);
	void SetHealth(float InHealth, float InMaxHealth);
	void SetShield(float InShield, float InMaxShield);
	void SetScore(float InScore);
	void SetDefeats(int32 InDefeats);
	void SetWeaponAmmo(int32 InAmmo);
	void SetCarriedAmmoEmpty();
	void SetCarriedAmmo(int32 InAmmo);
	void SetWeaponAmmoEmpty();
	void SetMatchCountdown(float InCountdownTime);
	void SetWarmupCountdown(float InCountDownTime);
	void SetAnnouncementText(FText InText);
	void SetAnnouncementInfoText(FText InText);
	void SetGrenadesAmount(int32 InGrenades);
};
