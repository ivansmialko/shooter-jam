// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UBackgroundBlur;
class UMainMenuCreateMatchWidget;
class UMainMenuFindMatchWidget;
struct FCreateWidgetData;
struct FCreateWidgetUserData;
struct FJoinWidgetData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuClickedHost);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuHostCancel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMenuHostCreate, const FCreateWidgetUserData&, CreateData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuClickedJoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuJoinJoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuJoinCancel);

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
	UPROPERTY(BlueprintCallable, Category = "Events")
	FMenuHostCancel OnMenuHostCancel;
	UPROPERTY(BlueprintCallable, Category = "Events")
	FMenuHostCreate OnMenuHostCreate;
	UPROPERTY(BlueprintCallable, Category = "Events")
	FMenuJoinJoin OnMenuJoinJoin;
	UPROPERTY(BlueprintCallable, Category = "Events")
	FMenuJoinCancel OnMenuJoinCancel;

//private fields
private:
	UPROPERTY(meta = (BindWidget))
	UMainMenuFindMatchWidget* FindMatchWidget;

	UPROPERTY(meta = (BindWidget))
	UMainMenuCreateMatchWidget* CreateMatchWidget;

	UPROPERTY(meta = (BindWidget))
	UBackgroundBlur* BackgroundBlur;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimShowBackgroundBlur;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimShowFindWidget;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimShowCreateWidget;

//public methods
public:
	void MenuSetup();

	void ShowCreateWidget(const FCreateWidgetData& InData);
	void HideCreateWidget();
	void ShowJoinWidget(const FJoinWidgetData& InData);
	void HideJoinWidget();

//private methods
private:
	UFUNCTION()
	void OnAnimationCreateWidgetFinishedHandler();
	UFUNCTION()
	void OnAnimationFindWidgetFinishedHandler();
	UFUNCTION()
	void OnAnimationBlurFinishedHandler();

};
