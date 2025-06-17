// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuClickedHost);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuClickedJoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuClickedTraining);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuClickedSettings);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuClickedExit);

UCLASS()
class SHOOTERJAM_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
//public fields
public:

	UPROPERTY(BlueprintCallable, Category = "Events")
	FMenuClickedHost OnMenuClickedHost;
	UPROPERTY(BlueprintCallable, Category = "Events")
	FMenuClickedJoin OnMenuClickedJoin;
	UPROPERTY(BlueprintCallable, Category = "Events")
	FMenuClickedTraining OnMenuClickedTraining;
	UPROPERTY(BlueprintCallable, Category = "Events")
	FMenuClickedSettings OnMenuClickedSettings;
	UPROPERTY(BlueprintCallable, Category = "Events")
	FMenuClickedExit OnMenuClickedExit;

//public methods
public:
	void MenuSetup();

//protected methods
protected:
	void InitializeButtonListeners();
};
