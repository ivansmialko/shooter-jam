// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

class UMainMenuWidget;

UCLASS()
class SHOOTERJAM_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> MainMenuBlueprint;

	UMainMenuWidget* MainMenuWidget;

//public methods
public:
	virtual void OnPossess(APawn* InPawn) override;

//protected methods
protected:
	void InitializeMenu();
	void InitializeListeners();
	void CreateMenu();

	UFUNCTION()
	void OnClickedHost();
	UFUNCTION()
	void OnClickedHostCreate();
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
};
