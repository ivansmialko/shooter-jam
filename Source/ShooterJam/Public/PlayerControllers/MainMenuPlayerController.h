// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "HUD/MainMenuCreateMatchWidget.h"

#include "MainMenuPlayerController.generated.h"

class UMainMenuWidget;
class UMultiplayerSessionsSubsystem;

UCLASS()
class SHOOTERJAM_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> MainMenuBlueprint;

	UMainMenuWidget* MainMenuWidget{ nullptr };

	UMultiplayerSessionsSubsystem* MultiplayerSubsystem{ nullptr };

//public methods
public:
	virtual void OnPossess(APawn* InPawn) override;

//protected methods
protected:
	void InitializeMenu();
	void InitializeListeners();
	void InitializeMultiplayerSubsystem();
	void CreateMenu();

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
