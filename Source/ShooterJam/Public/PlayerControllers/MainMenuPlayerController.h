// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "HUD/MainMenuCreateMatchWidget.h"
#include "HUD/MainMenuPreloaderWidget.h"

#include "MainMenuPlayerController.generated.h"

class UMainMenuWidget;
class UMainMenuPreloaderWidget;
class UMultiplayerSessionsSubsystem;

UCLASS()
class SHOOTERJAM_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> MainMenuBlueprint;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> MainMenuPreloaderBlueprint;

	UPROPERTY()
	UMainMenuWidget* MainMenuWidget;

	UPROPERTY()
	UMainMenuPreloaderWidget* MainMenuPreloaderWidget;

	UPROPERTY()
	UMultiplayerSessionsSubsystem* MultiplayerSubsystem;

	UPROPERTY(EditAnywhere)
	float MinimalPreloaderTime{ 3.f };

	float PreloaderTimer{ -1.f };

//public methods
public:
	virtual void OnPossess(APawn* InPawn) override;

	void ShowMenu() const;
	void HidePreloader() const;

	virtual void Tick(float InDeltaSeconds) override;

//protected methods
protected:
	void CreateMenu();

	void InitializeMenu();
	void InitializeListeners();
	void InitializeMultiplayerSubsystem();
	void InitializePreloader();

	void CheckHidePreloader() const;

	//Handlers for MainMenu
	UFUNCTION()
	void OnMenuCreateMatch(FCreateWidgetUserData InUserData);
	UFUNCTION()
	void OnMenuCreateMatchGetParams();
	UFUNCTION()
	void OnMenuFindMatchJoin(const FString& InSessionId);
	UFUNCTION()
	void OnMenuFindMatchGetParams();

	//Handlers for MultiplayerSubsystem
	UFUNCTION()
	void OnMpCreateSession(bool bWasSuccessfull);
	void OnMpFindSession(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessfull);
	void OnMpJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnMpDestroySession(bool bWasSuccessfull);
	UFUNCTION()
	void OnMpStartSession(bool bWasSuccessfull);
};
