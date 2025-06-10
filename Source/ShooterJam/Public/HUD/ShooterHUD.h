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
class UGameMenu;
class UWorldChat;
class UTeamBattleWidget;

UCLASS()
class SHOOTERJAM_API AShooterHUD : public AHUD
{
	GENERATED_BODY()
	
//private variables
private:
	//** Package for users crosshair images
	FCrosshairsPackage CrosshairsPackage;

	//** Instance of CharacterOverlay widget
	UCharacterOverlay* CharacterOverlay{ nullptr };

	//** Instance of Announcement widget
	UAnnouncementWidget* AnnouncementWidget{ nullptr };

	//** Instance of in-game menu */
	UGameMenu* GameMenuWidget{ nullptr };

	/** Instance of in-game chat */
	UWorldChat* WorldChatWidget{ nullptr };

	/** Instance of widget that overlays transition between matches */
	UUserWidget* TransitionOverlayWidget{ nullptr };

	//** Class of Overlay Widget to create
	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> CharacterOverlayBlueprint;

	//** Class of Announcement Widget to create
	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> AnnouncementWidgetBlueprint;

	/** Class of Game Menu Widget to create */
	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> GameMenuWidgetBlueprint;

	UPROPERTY(EditAnywhere, Category = "Chat")
	TSubclassOf<UUserWidget> WorldChatItemWidgetBlueprint;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> TransitionOverlayWidgetBlueprint;

	/** Max crosshair spread */
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax{ 60.f };

	UPROPERTY(EditAnywhere, Category = "Chat")
	float ChatMessageDuration{ 1.5f };

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
	/** Used to cache Carried Ammo amount */
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

	/** Creates an instance of UUserWidget and adds it to the screen */
	void AddGameMenuWidget();

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

	/** Shows in-game menu */
	void ShowGameMenu();

	/** Hides in-game menu */
	void HideGameMenu();

	/** Shows information about team progress on HUD */
	void ShowTeamBattleWidget();

	/** Hides information about team progress from HUD */
	void HideTeamBattleWidget();

	/** Shows emotion picker widget */
	void ShowEmotionPickerWidget();

	/** Hides emotion picker widget */
	void HideEmotionPickerWidget();

	/** Shows transition overlay widget */
	void ShowTransitionOverlayWidget();

//public getters/setters
public:
	bool GetIsGameMenuOpen();
	bool GetIsEmotionPickerOpen();
	UWorldChat* GetWorldChat() const;
	UTeamBattleWidget* GetTeamBattleWidget() const;

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
