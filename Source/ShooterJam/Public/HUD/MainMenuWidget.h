// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuClickedHost);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuHostCancel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuHostCreate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuClickedJoin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuClickedTraining);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuClickedSettings);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuClickedExit);


USTRUCT()
struct FCreateWidgetData
{
	GENERATED_BODY()

	FString LastMatchName;
	TArray<FString> MatchModesList;
};

USTRUCT()
struct FJoinWidgetItemData
{
	GENERATED_BODY()

	FString MatchMode;
	FName MatchName;
	uint32 Ping;
};

USTRUCT()
struct FJoinWidgetData
{
	GENERATED_BODY()
	TArray<FJoinWidgetItemData> MatchesList;
};

class UBackgroundBlur;
class UMainMenuCreateMatchWidget;

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

//private fields
private:
	UPROPERTY(meta = (BindWidget))
	UUserWidget* FindMatchWidget;

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

//private methods
private:
	UFUNCTION()
	void OnAnimationCreateWidgetFinishedHandler();
	UFUNCTION()
	void OnAnimationBlurFinishedHandler();
};
