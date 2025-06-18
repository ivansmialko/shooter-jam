// Made by smialko


#include "PlayerControllers/MainMenuPlayerController.h"

#include "HUD/MainMenuWidget.h"

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

	MainMenuWidget->OnMenuClickedHost.AddDynamic(this, &AMainMenuPlayerController::OnClickedHost);
	MainMenuWidget->OnMenuHostCreate.AddDynamic(this, &AMainMenuPlayerController::OnClickedHostCreate);
	MainMenuWidget->OnMenuHostCancel.AddDynamic(this, &AMainMenuPlayerController::OnClickedHostCancel);

	MainMenuWidget->OnMenuClickedJoin.AddDynamic(this, &AMainMenuPlayerController::OnClickedJoin);
	MainMenuWidget->OnMenuClickedTraining.AddDynamic(this, &AMainMenuPlayerController::OnClickedTraining);
	MainMenuWidget->OnMenuClickedSettings.AddDynamic(this, &AMainMenuPlayerController::OnClickedSettings);
	MainMenuWidget->OnMenuClickedExit.AddDynamic(this, &AMainMenuPlayerController::OnClickedExit);
}

void AMainMenuPlayerController::CreateMenu()
{
	InitializeMenu();
	InitializeListeners();
}

void AMainMenuPlayerController::OnClickedHost()
{
	if (!MainMenuWidget)
		return;

	FCreateWidgetData WidgetData;
	WidgetData.LastMatchName = "Pdidy party";
	WidgetData.MatchModesList.Add("Deathmatch");
	WidgetData.MatchModesList.Add("Teams");
	MainMenuWidget->ShowCreateWidget(WidgetData);
}

void AMainMenuPlayerController::OnClickedHostCreate()
{
	if (!MainMenuWidget)
		return;

	MainMenuWidget->HideCreateWidget();
}

void AMainMenuPlayerController::OnClickedHostCancel()
{
	if (!MainMenuWidget)
		return;

	MainMenuWidget->HideCreateWidget();
}

void AMainMenuPlayerController::OnClickedJoin()
{
	UE_LOG(LogTemp, Warning, TEXT("Clicked join"));
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
