// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUD/MainMenuCreateMatchWidget.h"
#include "HUD/MainMenuFindMatchWidget.h"

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
	DECLARE_DELEGATE(FMainMenuFindGetParams);
	
//public fields
public:
	FMainMenuCreate OnCreateMatchDlg;
	FMainMenuCreateGetParams OnCreateMatchGetParamsDlg;
	FMainMenuFindGetParams OnFindMatchGetParamsDlg;

//private fields
private:
	UPROPERTY(meta = (BindWidget))
	UMainMenuCreateMatchWidget* CreateMatchWidget;

	UPROPERTY(meta = (BindWidget))
	UMainMenuFindMatchWidget* FindMatchWidget;

	UPROPERTY(meta = (BindWidget))
	UBackgroundBlur* BackgroundBlur;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimShowBackgroundBlur;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimShowCreateWidget;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimShowFindWidget;

//public methods
public:
	void NativeConstruct();
	void InitializeListeners();

	void SetCreateParams(const FCreateWidgetData& InCreateParams);
	void SetFindParams(const FFindWidgetData& InFindParams);

	UFUNCTION(BlueprintCallable)
	void ShowCreateWidget();
	UFUNCTION(BlueprintCallable)
	void ShowFindWidget();
	void HideCreateWidget();
	void HideFindWidget();

//private methods
private:

	UFUNCTION()
	void OnCreateMatchCreate();
	UFUNCTION()
	void OnCreateMatchClose();
	UFUNCTION()
	void OnFindMatchClose();

	UFUNCTION()
	void OnAnimationCreateWidgetFinishedHandler();
	UFUNCTION()
	void OnAnimationFindWidgetFinishedHandler();
	UFUNCTION()
	void OnAnimationBlurFinishedHandler();

};
