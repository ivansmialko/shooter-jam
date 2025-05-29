// Made by smialko


#include "HUD/GameMenu.h"

#include "Characters/ShooterCharacter.h"

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

	ShooterCharacterController = (!ShooterCharacterController ? World->GetFirstPlayerController() : ShooterCharacterController);

	FInputModeGameAndUI InputModeData;
	InputModeData.SetWidgetToFocus(TakeWidget());

	ShooterCharacterController->SetInputMode(InputModeData);
	ShooterCharacterController->SetShowMouseCursor(true);

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
		return;

	MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	if (MultiplayerSessionsSubsystem && !MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsBound())
	{
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UGameMenu::OnDestroySession);
	}

	if (ExitButton && !ExitButton->OnClicked.IsBound())
	{
		ExitButton->OnClicked.AddDynamic(this, &UGameMenu::OnExitButtonClicked);
	}
}

void UGameMenu::MenuTeardown()
{
	RemoveFromParent();

	UWorld* World{ GetWorld() };
	if (!World)
		return;

	ShooterCharacterController = (!ShooterCharacterController ? World->GetFirstPlayerController() : ShooterCharacterController);

	FInputModeGameOnly InputModeData;
	ShooterCharacterController->SetInputMode(InputModeData);
	ShooterCharacterController->SetShowMouseCursor(false);

	if (ExitButton)
	{
		ExitButton->OnClicked.RemoveDynamic(this, &UGameMenu::OnExitButtonClicked);
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &UGameMenu::OnDestroySession);
	}
}

bool UGameMenu::Initialize()
{
	if (!Super::Initialize())
		return false;

	return true;
}

void UGameMenu::OnDestroySession(bool bWasSuccessfull)
{
	ExitButton->SetIsEnabled(true);

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
		ShooterCharacterController = (!ShooterCharacterController ? World->GetFirstPlayerController() : ShooterCharacterController);
		if (ShooterCharacterController)
		{
			ShooterCharacterController->ClientReturnToMainMenuWithTextReason(FText());
		}
	}
}

void UGameMenu::OnPlayerLeft()
{
	if (!MultiplayerSessionsSubsystem)
		return;

	MultiplayerSessionsSubsystem->DestroySession();
}

void UGameMenu::OnExitButtonClicked()
{
	ExitButton->SetIsEnabled(false);

	UWorld* World{ GetWorld() };
	if (!World)
		return;

	APlayerController* Controller { World->GetFirstPlayerController() };
	if (!Controller)
		return;

	AShooterCharacter* ShooterCharacterLeaving{ Cast<AShooterCharacter>(Controller->GetPawn()) };
	if (ShooterCharacterLeaving)
	{
		ShooterCharacterLeaving->Server_LeaveGame();
		ShooterCharacterLeaving->OnLeftGame.AddDynamic(this, &UGameMenu::OnPlayerLeft);
	}
	else
	{
		ExitButton->SetIsEnabled(true);
	}

}
