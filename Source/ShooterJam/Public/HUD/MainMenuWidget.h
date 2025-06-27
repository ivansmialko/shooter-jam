// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUD/MainMenuCreateMatchWidget.h"

#include "MainMenuWidget.generated.h"

class UBackgroundBlur;
class UMainMenuCreateMatchWidget;
class UMainMenuFindMatchWidget;



UCLASS()
class SHOOTERJAM_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

	DECLARE_DELEGATE_OneParam(FMainMenuCreate, FCreateWidgetUserData);
	DECLARE_DELEGATE(FMainMenuCreateGetParams);
	
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
	void NativeConstruct();

	void SetCreateParams(const FCreateWidgetData& InCreateParams);

//private methods
private:
	void InitializeListeners();

	UFUNCTION(BlueprintCallable)
	void ShowCreateWidget();
	UFUNCTION(BlueprintCallable)
	void HideCreateWidget();
	//void ShowJoinWidget(const FJoinWidgetData& InData);
	//void HideJoinWidget();

	UFUNCTION()
	void OnCreateMatchCreate();
	UFUNCTION()
	void OnCreateMatchClose();

	UFUNCTION()
	void OnAnimationCreateWidgetFinishedHandler();
	//UFUNCTION()
	//void OnAnimationFindWidgetFinishedHandler();
	UFUNCTION()
	void OnAnimationBlurFinishedHandler();

};
