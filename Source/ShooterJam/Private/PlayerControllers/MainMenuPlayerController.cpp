// Made by smialko


#include "PlayerControllers/MainMenuPlayerController.h"

#include "MultiplayerSessionsSubsystem.h"

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

	MainMenuWidget->MenuSetup();

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

	MainMenuWidget->OnCreateMatchDlg.AddDynamic(this, &AMainMenuPlayerController::On)

	//MainMenuWidget->OnMenuClickedHost.AddDynamic(this, &AMainMenuPlayerController::OnClickedHost);
	//MainMenuWidget->OnMenuHostCreate.AddDynamic(this, &AMainMenuPlayerController::OnClickedHostCreate);
	//MainMenuWidget->OnMenuHostCancel.AddDynamic(this, &AMainMenuPlayerController::OnClickedHostCancel);

	//MainMenuWidget->OnMenuClickedJoin.AddDynamic(this, &AMainMenuPlayerController::OnClickedJoin);
	//MainMenuWidget->OnMenuJoinJoin.AddDynamic(this, &AMainMenuPlayerController::OnClickedJoinJoin);
	//MainMenuWidget->OnMenuJoinCancel.AddDynamic(this, &AMainMenuPlayerController::OnClickedJoinCancel);

	//MainMenuWidget->OnMenuClickedTraining.AddDynamic(this, &AMainMenuPlayerController::OnClickedTraining);
	//MainMenuWidget->OnMenuClickedSettings.AddDynamic(this, &AMainMenuPlayerController::OnClickedSettings);
	//MainMenuWidget->OnMenuClickedExit.AddDynamic(this, &AMainMenuPlayerController::OnClickedExit);
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

	MultiplayerSubsystem->CreateSession(InUserData.MaxPlayers, InUserData.MatchMode);
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

void AMainMenuPlayerController::OnClickedHostCreate()
{
	if (!MainMenuWidget)
		return;

	//MainMenuWidget->HideCreateWidget();

	if (!MultiplayerSubsystem)
		return;

	FCreateWidgetUserData CreateSessionData;
	if (!MainMenuWidget->GetCreateWidgetUserData(CreateSessionData))
		return;

	MultiplayerSubsystem->CreateSession(CreateSessionData.MaxPlayers, CreateSessionData.MatchMode);
}

void AMainMenuPlayerController::OnClickedHostCancel()
{
	if (!MainMenuWidget)
		return;

	MainMenuWidget->HideCreateWidget();
}

void AMainMenuPlayerController::OnClickedJoin()
{
	if (!MainMenuWidget)
		return;

	FJoinWidgetData JoinWidget;
	JoinWidget.MatchesList.Add(FJoinWidgetItemData("DeathMatch", "PdidyParty", 23));
	JoinWidget.MatchesList.Add(FJoinWidgetItemData("DeathMatch", "PdidyParty", 23));
	JoinWidget.MatchesList.Add(FJoinWidgetItemData("DeathMatch", "PdidyParty", 23));
	JoinWidget.MatchesList.Add(FJoinWidgetItemData("DeathMatch", "PdidyParty", 23));
	JoinWidget.MatchesList.Add(FJoinWidgetItemData("DeathMatch", "PdidyParty", 23));
	JoinWidget.MatchesList.Add(FJoinWidgetItemData("DeathMatch", "PdidyParty", 23));
	JoinWidget.MatchesList.Add(FJoinWidgetItemData("DeathMatch", "PdidyParty", 23));
	JoinWidget.MatchesList.Add(FJoinWidgetItemData("DeathMatch", "Epstein Island", 23));
	JoinWidget.MatchesList.Add(FJoinWidgetItemData("DeathMatch", "Epstein Island", 23));
	JoinWidget.MatchesList.Add(FJoinWidgetItemData("DeathMatch", "Epstein Island", 23));
	JoinWidget.MatchesList.Add(FJoinWidgetItemData("DeathMatch", "Epstein Island", 23));
	JoinWidget.MatchesList.Add(FJoinWidgetItemData("DeathMatch", "Epstein Island", 23));

	//MainMenuWidget->ShowJoinWidget(JoinWidget);
}

void AMainMenuPlayerController::OnClickedJoinJoin()
{
	if (!MainMenuWidget)
		return;

	//MainMenuWidget->HideJoinWidget();
}

void AMainMenuPlayerController::OnClickedJoinCancel()
{
	if (!MainMenuWidget)
		return;

	//MainMenuWidget->HideJoinWidget();
}

void AMainMenuPlayerController::OnClickedTraining()
{
	UE_LOG(LogTemp, Warning, TEXT("Clicked training"));
}

void AMainMenuPlayerController::OnClickedSettings()
{
	UE_LOG(LogTemp, Warning, TEXT("Clicked settings"));
}

void AMainMenuPlayerController::OnClickedExit()
{
	UE_LOG(LogTemp, Warning, TEXT("Clicked exit"));
}

void AMainMenuPlayerController::OnMpCreateSession(bool bWasSuccessfull)
{

}

void AMainMenuPlayerController::OnMpFindSession(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessfull)
{

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
