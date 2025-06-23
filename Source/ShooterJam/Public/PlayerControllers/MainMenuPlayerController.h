// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

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

	UFUNCTION()
	void OnClickedHost();
	UFUNCTION()
	void OnClickedHostCreate(const FCreateWidgetUserData& InCreateData);
	UFUNCTION()
	void OnClickedHostCancel();
	UFUNCTION()
	void OnClickedJoin();
	UFUNCTION()
	void OnClickedJoinJoin();
	UFUNCTION()
	void OnClickedJoinCancel();
	UFUNCTION()
	void OnClickedTraining();
	UFUNCTION()
	void OnClickedSettings();
	UFUNCTION()
	void OnClickedExit();

	UFUNCTION()
	void OnMpCreateSession(bool bWasSuccessfull);
	UFUNCTION()
	void OnMpFindSession(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessfull);
	UFUNCTION()
	void OnMpJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnMpDestroySession(bool bWasSuccessfull);
	UFUNCTION()
	void OnMpStartSession(bool bWasSuccessfull);
};
