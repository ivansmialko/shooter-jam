// Made by smialko


#include "HUD/GameMenu.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"

void UGameMenu::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	UWorld* World{ GetWorld() };
	if (!World)
		return;

	PlayerController = (!PlayerController ? World->GetFirstPlayerController() : PlayerController);

	FInputModeGameAndUI InputModeData;
	InputModeData.SetWidgetToFocus(TakeWidget());

	PlayerController->SetInputMode(InputModeData);
	PlayerController->SetShowMouseCursor(true);

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
		return;

	MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	if (!MultiplayerSessionsSubsystem)
		return;

	MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UGameMenu::OnDestroySession);
}

void UGameMenu::MenuTeardown()
{
	RemoveFromParent();

	UWorld* World{ GetWorld() };
	if (!World)
		return;

	PlayerController = (!PlayerController ? World->GetFirstPlayerController() : PlayerController);

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	PlayerController->SetShowMouseCursor(false);
}

bool UGameMenu::Initialize()
{
	if (!Super::Initialize())
		return false;

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UGameMenu::OnExitButtonClicked);
	}

	return true;
}

void UGameMenu::OnDestroySession(bool bWasSuccessfull)
{
	if (!bWasSuccessfull)
	{
		ExitButton->SetIsEnabled(true);
		return;
	}

	UWorld* World{ GetWorld() };
	if (!World)
		return;

	AGameModeBase* GameMode{ World->GetAuthGameMode<AGameModeBase>() };
	if (GameMode)
	{
		GameMode->ReturnToMainMenuHost();
	}
	else
	{
		PlayerController = (!PlayerController ? World->GetFirstPlayerController() : PlayerController);
		if (PlayerController)
		{
			PlayerController->ClientReturnToMainMenuWithTextReason(FText());
		}
	}
}

void UGameMenu::OnExitButtonClicked()
{
	ExitButton->SetIsEnabled(false);

	if (!MultiplayerSessionsSubsystem)
		return;

	MultiplayerSessionsSubsystem->DestroySession();
}
