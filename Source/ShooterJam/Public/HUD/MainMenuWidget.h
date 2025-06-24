// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUD/MainMenuCreateMatchWidget.h"

#include "MainMenuWidget.generated.h"

class UBackgroundBlur;
class UMainMenuCreateMatchWidget;
class UMainMenuFindMatchWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMainMenuCreate, FCreateWidgetUserData, CreateSessionData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMainMenuCreateGetParams);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuJoinJoin);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuJoinCancel);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuClickedTraining);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuClickedSettings);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuClickedExit);

UCLASS()
class SHOOTERJAM_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
//public fields
public:
	FMainMenuCreate OnCreateMatchDlg;
	FMainMenuCreateGetParams OnCreateMatchGetParamsDlg;

	//UPROPERTY(BlueprintCallable, Category = "Events")
	//FMenuClickedHost OnMenuClickedHost;
	//UPROPERTY(BlueprintCallable, Category = "Events")
	//FMenuClickedJoin OnMenuClickedJoin;
	//UPROPERTY(BlueprintCallable, Category = "Events")
	//FMenuClickedTraining OnMenuClickedTraining;
	//UPROPERTY(BlueprintCallable, Category = "Events")
	//FMenuClickedSettings OnMenuClickedSettings;
	//UPROPERTY(BlueprintCallable, Category = "Events")
	//FMenuClickedExit OnMenuClickedExit;
	//UPROPERTY(BlueprintCallable, Category = "Events")
	//FMenuHostCancel OnMenuHostCancel;
	//UPROPERTY(BlueprintCallable, Category = "Events")
	//FMenuHostCreate OnMenuHostCreate;
	//UPROPERTY(BlueprintCallable, Category = "Events")
	//FMenuJoinJoin OnMenuJoinJoin;
	//UPROPERTY(BlueprintCallable, Category = "Events")
	//FMenuJoinCancel OnMenuJoinCancel;

//private fields
private:
	//UPROPERTY(meta = (BindWidget))
	//UMainMenuFindMatchWidget* FindMatchWidget;

	UPROPERTY(meta = (BindWidget))
	UMainMenuCreateMatchWidget* CreateMatchWidget;

	UPROPERTY(meta = (BindWidget))
	UBackgroundBlur* BackgroundBlur;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimShowBackgroundBlur;

	//UPROPERTY(meta = (BindWidgetAnim), Transient)
	//UWidgetAnimation* AnimShowFindWidget;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimShowCreateWidget;

//public methods
public:
	void SetCreateParams(const FCreateWidgetData& InCreateParams);

//private methods
private:

	void MenuSetup();
	void InitializeListeners();

	void ShowCreateWidget();
	void HideCreateWidget();
	//void ShowJoinWidget(const FJoinWidgetData& InData);
	//void HideJoinWidget();

	UFUNCTION()
	void OnCreateMatchCreate(FCreateWidgetUserData InUserData);
	UFUNCTION()
	void OnCreateMatchClose();

	UFUNCTION()
	void OnAnimationCreateWidgetFinishedHandler();
	//UFUNCTION()
	//void OnAnimationFindWidgetFinishedHandler();
	UFUNCTION()
	void OnAnimationBlurFinishedHandler();

};
