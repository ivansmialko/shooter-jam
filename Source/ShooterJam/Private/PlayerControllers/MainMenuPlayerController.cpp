// Made by smialko


#include "PlayerControllers/MainMenuPlayerController.h"

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"

#include "HUD/MainMenuWidget.h"
#include "HUD/MainMenuCreateMatchWidget.h"
#include "HUD/MainMenuFindMatchWidget.h"
#include "HUD/MainMenuFindMatchWidgetItem.h"

void AMainMenuPlayerController::OnPossess(APawn* InPawn)
{
	CreateMenu();
}

void AMainMenuPlayerController::ShowMenu() const
{
	if (!MainMenuWidget)
		return;

	MainMenuWidget->SetVisibility(ESlateVisibility::Visible);

	if (MultiplayerSubsystem && MultiplayerSubsystem->GetOnlineSubsystemAvailable() && !MultiplayerSubsystem->GetIsLanMatch())
	{
		MainMenuWidget->DisableMatchmakingButtons();
	}
}

void AMainMenuPlayerController::HidePreloader() const
{
	if (!MainMenuPreloaderWidget)
		return;

	MainMenuPreloaderWidget->SetVisibility(ESlateVisibility::Hidden);
}

void AMainMenuPlayerController::Tick(float InDeltaSeconds)
{
	Super::Tick(InDeltaSeconds);

	CheckHidePreloader();
}

void AMainMenuPlayerController::InitializeMenu()
{
	if (!MainMenuBlueprint)
		return;

	MainMenuWidget = CreateWidget<UMainMenuWidget>(this, MainMenuBlueprint);
	if (!MainMenuWidget)
		return;

	MainMenuWidget->SetIsFocusable(true);
	MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);
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
	MainMenuWidget->OnJoinMatchDlg.BindUFunction(this, FName("OnMenuFindMatchJoin"));
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

void AMainMenuPlayerController::InitializePreloader()
{
	if (!MainMenuPreloaderBlueprint)
		return;

	MainMenuPreloaderWidget = CreateWidget<UMainMenuPreloaderWidget>(this, MainMenuPreloaderBlueprint);
	if (!MainMenuPreloaderWidget)
		return;

	MainMenuPreloaderWidget->SetVisibility(ESlateVisibility::Visible);
	MainMenuPreloaderWidget->AddToViewport();
}

void AMainMenuPlayerController::CheckHidePreloader() const
{
	bool bPreloaderActive = true;
	bPreloaderActive = bPreloaderActive && (MinimalPreloaderTime != 0);
	bPreloaderActive = bPreloaderActive && (!MainMenuWidget || MainMenuWidget->GetVisibility() == ESlateVisibility::Visible);
	bPreloaderActive = bPreloaderActive && (!MultiplayerSubsystem || MultiplayerSubsystem->GetOnlineSubsystemAvailable());

#if WITH_EDITOR
	bNeedHidePreloader = false;
#endif

	if (!bPreloaderActive)
	{
		HidePreloader();
		ShowMenu();
	}

}

void AMainMenuPlayerController::CreateMenu()
{
	InitializePreloader();
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
	WidgetData.LastMatchName = "Multiplayer party";
	WidgetData.MatchModesList.Add("Death match");
	MainMenuWidget->SetCreateParams(WidgetData);
}

void AMainMenuPlayerController::OnMenuFindMatchJoin(const FString& InSessionId)
{
	if (!MultiplayerSubsystem)
		return;

	MultiplayerSubsystem->JoinSession(InSessionId);
}

void AMainMenuPlayerController::OnMenuFindMatchGetParams()
{
#if WITH_EDITOR
	if (!MainMenuWidget)
		return;

	FFindWidgetData FindWidgetData;
	FindWidgetData.MatchesList.Add(FFindWidgetItemData("TestMatch 1", "Testik", 9, "TestSessionId1"));
	FindWidgetData.MatchesList.Add(FFindWidgetItemData("TestMatch 2", "Testik", 9, "TestSessionId2"));
	FindWidgetData.MatchesList.Add(FFindWidgetItemData("TestMatch 3", "Testik", 9, "TestSessionId3"));
	FindWidgetData.MatchesList.Add(FFindWidgetItemData("TestMatch 4", "Testik", 9, "TestSessionId4"));

	MainMenuWidget->SetFindParams(FindWidgetData);
#else
	if (!MultiplayerSubsystem)
		return;

	MultiplayerSubsystem->FindSessions(200000);
#endif // WITH_EDITOR
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
	if (!MultiplayerSubsystem)
		return;

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, FString("Failed to join session"));
		return;
	}

	APlayerController* PlayerController{ GetGameInstance()->GetFirstLocalPlayerController() };
	if (!PlayerController)
		return;

	PlayerController->ClientTravel(MultiplayerSubsystem->GetSessionAddress(), ETravelType::TRAVEL_Absolute);
}

void AMainMenuPlayerController::OnMpDestroySession(bool bWasSuccessfull)
{

}

void AMainMenuPlayerController::OnMpStartSession(bool bWasSuccessfull)
{

}
