// Made by smialko


#include "PlayerControllers/MainMenuPlayerController.h"

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"

#include "HUD/MainMenuWidget.h"
#include "HUD/MainMenuCreateMatchWidget.h"
#include "HUD/MainMenuFindMatchWidget.h"
#include "HUD/MainMenuFindMatchWidgetItem.h"

void AMainMenuPlayerController::OnPossess(APawn* InPawn)
{
	CreateMenu();
}

void AMainMenuPlayerController::InitializeMenu()
{
	if (!MainMenuBlueprint)
		return;

	MainMenuWidget = CreateWidget<UMainMenuWidget>(this, MainMenuBlueprint);
	if (!MainMenuWidget)
		return;

	MainMenuWidget->SetIsFocusable(true);
	MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	MainMenuWidget->AddToViewport();

	if (!MainMenuWidget)
		return;

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(MainMenuWidget->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(InputModeData);
	SetShowMouseCursor(true);
}

void AMainMenuPlayerController::InitializeListeners()
{
	if (!MainMenuWidget)
		return;

	MainMenuWidget->OnCreateMatchDlg.BindUFunction(this, FName("OnMenuCreateMatch"));
	MainMenuWidget->OnCreateMatchGetParamsDlg.BindUFunction(this, FName("OnMenuCreateMatchGetParams"));
	MainMenuWidget->OnFindMatchGetParamsDlg.BindUFunction(this, FName("OnMenuFindMatchGetParams"));
}

void AMainMenuPlayerController::InitializeMultiplayerSubsystem()
{
	if (MultiplayerSubsystem)
		return;

	UGameInstance* GameInstance{ GetGameInstance() };
	if (!GameInstance)
		return;

	MultiplayerSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	if (!MultiplayerSubsystem)
		return;

	MultiplayerSubsystem->SetLogToScreen(true);
	MultiplayerSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &AMainMenuPlayerController::OnMpCreateSession);
	MultiplayerSubsystem->MultiplayerOnFindSessionComplete.AddUObject(this, &AMainMenuPlayerController::OnMpFindSession);
	MultiplayerSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &AMainMenuPlayerController::OnMpJoinSession);
	MultiplayerSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &AMainMenuPlayerController::OnMpDestroySession);
	MultiplayerSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &AMainMenuPlayerController::OnMpStartSession);
}

void AMainMenuPlayerController::CreateMenu()
{
	InitializeMenu();
	InitializeListeners();
	InitializeMultiplayerSubsystem();
}

void AMainMenuPlayerController::OnMenuCreateMatch(FCreateWidgetUserData InUserData)
{
	if (!MultiplayerSubsystem)
		return;

	FMultiplayerMatchSettings MatchSettings;
	MatchSettings.PublicConnections = InUserData.MaxPlayers;
	MatchSettings.MatchType = InUserData.MatchMode;
	MatchSettings.MatchName = InUserData.MatchName;

	MultiplayerSubsystem->CreateSession(MatchSettings);
}

void AMainMenuPlayerController::OnMenuCreateMatchGetParams()
{
	//TODO: Get last user's settings from save
	FCreateWidgetData WidgetData;
	WidgetData.LastMatchName = "Test party";
	WidgetData.MatchModesList.Add("Death match");
	WidgetData.MatchModesList.Add("Teams");
	MainMenuWidget->SetCreateParams(WidgetData);
}

void AMainMenuPlayerController::OnMenuFindMatchGetParams()
{
	if (!MultiplayerSubsystem)
		return;

	MultiplayerSubsystem->FindSessions(200000);
}

void AMainMenuPlayerController::OnMpCreateSession(bool bWasSuccessfull)
{
	if (!bWasSuccessfull)
	{
		GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, FString("Failed to create session"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Green, FString("Match created, traveling to map"));
	const FString MatchMap("/Game/Maps/CityMap_Deathmatch?listen");

	UWorld* World = GetWorld();
	if (!World)
		return;

	World->ServerTravel(MatchMap);

}

void AMainMenuPlayerController::OnMpFindSession(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessfull)
{
	if (!bWasSuccessfull)
	{
		if (SearchResults.IsEmpty())
		{
			GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, FString("No sessions found"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, FString("Search failed"));

		}
		MainMenuWidget->HideFindWidget();

		return;
	}

	GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, FString::Printf(TEXT("Found %d sessions"), SearchResults.Num()));

	FFindWidgetData WidgetData;
	for (const FOnlineSessionSearchResult& SearchResult : SearchResults)
	{
		FString MatchType;
		SearchResult.Session.SessionSettings.Get(FName("MatchType"), MatchType);

		FString MatchName;
		SearchResult.Session.SessionSettings.Get(FName("MatchName"), MatchName);

		WidgetData.MatchesList.Add(FFindWidgetItemData(MatchType, MatchName, SearchResult.PingInMs, SearchResult.GetSessionIdStr()));
	}

	if(!MainMenuWidget)
		return;

	MainMenuWidget->SetFindParams(WidgetData);
}

void AMainMenuPlayerController::OnMpJoinSession(EOnJoinSessionCompleteResult::Type Result)
{

}

void AMainMenuPlayerController::OnMpDestroySession(bool bWasSuccessfull)
{

}

void AMainMenuPlayerController::OnMpStartSession(bool bWasSuccessfull)
{

}
