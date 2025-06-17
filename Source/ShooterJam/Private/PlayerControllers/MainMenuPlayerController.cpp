// Made by smialko


#include "PlayerControllers/MainMenuPlayerController.h"

#include "HUD/MainMenuWidget.h"

void AMainMenuPlayerController::OnPossess(APawn* InPawn)
{
	CreateMenu();
	InitializeInput();
}

void AMainMenuPlayerController::InitializeInput()
{
	if (!MainMenuWidget)
		return;

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(MainMenuWidget->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(InputModeData);
	SetShowMouseCursor(true);
}

void AMainMenuPlayerController::CreateMenu()
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

	if (!MainMenuWidget->OnMenuClickedHost.IsBound())
	{
		MainMenuWidget->OnMenuClickedHost.AddDynamic(this, &AMainMenuPlayerController::OnClickedHost);
	}

	if (!MainMenuWidget->OnMenuClickedJoin.IsBound())
	{
		MainMenuWidget->OnMenuClickedJoin.AddDynamic(this, &AMainMenuPlayerController::OnClickedJoin);
	}

	if (!MainMenuWidget->OnMenuClickedTraining.IsBound())
	{
		MainMenuWidget->OnMenuClickedTraining.AddDynamic(this, &AMainMenuPlayerController::OnClickedTraining);
	}

	if (!MainMenuWidget->OnMenuClickedSettings.IsBound())
	{
		MainMenuWidget->OnMenuClickedSettings.AddDynamic(this, &AMainMenuPlayerController::OnClickedSettings);
	}

	if (!MainMenuWidget->OnMenuClickedExit.IsBound())
	{
		MainMenuWidget->OnMenuClickedExit.AddDynamic(this, &AMainMenuPlayerController::OnClickedExit);
	}
}

void AMainMenuPlayerController::OnClickedHost()
{
	UE_LOG(LogTemp, Warning, TEXT("Clicked host"));
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
