// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMenu.generated.h"

class UButton;
class UMultiplayerSessionsSubsystem;

UCLASS()
class SHOOTERJAM_API UGameMenu : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	APlayerController* PlayerController{ nullptr };
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem{ nullptr };

public:
	void MenuSetup();
	void MenuTeardown(); 

protected:
	virtual bool Initialize() override;

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessfull);
	UFUNCTION()
	void OnPlayerLeft();

private:

	UFUNCTION()
	void OnExitButtonClicked();
};
