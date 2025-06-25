// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUD/MainMenuCreateMatchWidget.h"

#include "MainMenuWidget.generated.h"

class UBackgroundBlur;
class UMainMenuCreateMatchWidget;
class UMainMenuFindMatchWidget;

DECLARE_DYNAMIC_DELEGATE_OneParam(FMainMenuCreate, FCreateWidgetUserData, CreateSessionData);
DECLARE_DYNAMIC_DELEGATE(FMainMenuCreateGetParams);

UCLASS()
class SHOOTERJAM_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
//public fields
public:
	FMainMenuCreate OnCreateMatchDlg;
	FMainMenuCreateGetParams OnCreateMatchGetParamsDlg;


//private fields
private:
	UPROPERTY(meta = (BindWidget))
	UMainMenuCreateMatchWidget* CreateMatchWidget;

	UPROPERTY(meta = (BindWidget))
	UBackgroundBlur* BackgroundBlur;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimShowBackgroundBlur;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimShowCreateWidget;

//public methods
public:
	void SetCreateParams(const FCreateWidgetData& InCreateParams);

//private methods
private:

	void MenuSetup();
	void InitializeListeners();

	UFUNCTION(BlueprintCallable)
	void ShowCreateWidget();
	UFUNCTION(BlueprintCallable)
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
