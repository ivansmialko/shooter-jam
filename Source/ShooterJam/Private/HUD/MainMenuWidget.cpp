// Made by smialko


#include "HUD/MainMenuWidget.h"

#include "HUD/MainMenuCreateMatchWidget.h"
#include "HUD/MainMenuFindMatchWidget.h"

#include "Components/BackgroundBlur.h"
#include "Components/Widget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UMainMenuWidget::NativeConstruct()
{
	InitializeListeners();
}

void UMainMenuWidget::SetCreateParams(const FCreateWidgetData& InCreateParams)
{
	if (!CreateMatchWidget)
		return;

	CreateMatchWidget->SetWidgetData(InCreateParams);
}

void UMainMenuWidget::SetFindParams(const FFindWidgetData& InFindParams)
{
	if (!FindMatchWidget)
		return;

	FindMatchWidget->SetWidgetData(InFindParams);
}

void UMainMenuWidget::InitializeListeners()
{
	if (CreateMatchWidget)
	{
		CreateMatchWidget->OnCreateDlg.AddDynamic(this, &UMainMenuWidget::OnCreateMatchCreate);
		CreateMatchWidget->OnCloseDlg.AddDynamic(this, &UMainMenuWidget::OnCreateMatchClose);
		FindMatchWidget->OnJoinDlg.AddDynamic(this, &UMainMenuWidget::OnFindMatchJoin);
		FindMatchWidget->OnCloseDlg.AddDynamic(this, &UMainMenuWidget::OnFindMatchClose);
	}
}

void UMainMenuWidget::ShowCreateWidget()
{
	if (!BackgroundBlur)
		return;

	if (!AnimShowBackgroundBlur)
		return;

	if (!CreateMatchWidget)
		return;
	
	BackgroundBlur->SetVisibility(ESlateVisibility::Visible);
	CreateMatchWidget->SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(AnimShowBackgroundBlur);
	PlayAnimation(AnimShowCreateWidget);

	//Request data from whoever is controlling this menu
	OnCreateMatchGetParamsDlg.ExecuteIfBound();
}

void UMainMenuWidget::HideCreateWidget()
{
	if (!AnimShowBackgroundBlur)
		return;

	if (!AnimShowCreateWidget)
		return;

	PlayAnimation(AnimShowBackgroundBlur, 0.f, 1, EUMGSequencePlayMode::Reverse);
	PlayAnimation(AnimShowCreateWidget, 0.f, 1, EUMGSequencePlayMode::Reverse);

	FWidgetAnimationDynamicEvent OnAnimationBlurFinished;
	OnAnimationBlurFinished.BindDynamic(this, &UMainMenuWidget::OnAnimationBlurFinishedHandler);
	FWidgetAnimationDynamicEvent OnAnimationCreateWidgetFinished;
	OnAnimationCreateWidgetFinished.BindDynamic(this, &UMainMenuWidget::OnAnimationCreateWidgetFinishedHandler);

	BindToAnimationFinished(AnimShowBackgroundBlur, OnAnimationBlurFinished);
	BindToAnimationFinished(AnimShowCreateWidget, OnAnimationCreateWidgetFinished);
}

void UMainMenuWidget::ShowFindWidget()
{
	if (!FindMatchWidget)
		return;

	if (!AnimShowFindWidget)
		return;

	if (!AnimShowBackgroundBlur)
		return;

	FindMatchWidget->SetVisibility(ESlateVisibility::Visible);
	BackgroundBlur->SetVisibility(ESlateVisibility::Visible);

	PlayAnimation(AnimShowFindWidget);
	PlayAnimation(AnimShowBackgroundBlur);

	OnFindMatchGetParamsDlg.ExecuteIfBound();
}

void UMainMenuWidget::HideFindWidget()
{
	if (!AnimShowBackgroundBlur)
		return;

	if (!AnimShowFindWidget)
		return;

	PlayAnimation(AnimShowBackgroundBlur, 0.f, 1, EUMGSequencePlayMode::Reverse);
	PlayAnimation(AnimShowFindWidget, 0.f, 1, EUMGSequencePlayMode::Reverse);

	FWidgetAnimationDynamicEvent OnAnimationBlurFinished;
	OnAnimationBlurFinished.BindDynamic(this, &UMainMenuWidget::OnAnimationBlurFinishedHandler);
	FWidgetAnimationDynamicEvent OnAnimationFindWidgetFinished;
	OnAnimationFindWidgetFinished.BindDynamic(this, &UMainMenuWidget::OnAnimationFindWidgetFinishedHandler);

	BindToAnimationFinished(AnimShowBackgroundBlur, OnAnimationBlurFinished);
	BindToAnimationFinished(AnimShowFindWidget, OnAnimationFindWidgetFinished);
}

void UMainMenuWidget::DisableMatchmakingButtons()
{
	if (ButtonCreateMatch)
	{
		ButtonCreateMatch->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (ButtonFindMatch)
	{
		ButtonFindMatch->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (TextSteamRestartGame)
	{
		TextSteamRestartGame->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMainMenuWidget::OnCreateMatchCreate()
{
	if (!CreateMatchWidget)
		return;

	FCreateWidgetUserData UserData;
	CreateMatchWidget->GetUserData(UserData);

	OnCreateMatchDlg.ExecuteIfBound(UserData);

}

void UMainMenuWidget::OnCreateMatchClose()
{
	HideCreateWidget();
}

void UMainMenuWidget::OnFindMatchJoin()
{
	if (!FindMatchWidget)
		return;

	FString SessionId;
	if (FindMatchWidget->GetSessionId(SessionId))
	{
		OnJoinMatchDlg.ExecuteIfBound(SessionId);
	}
}

void UMainMenuWidget::OnFindMatchClose()
{
	HideFindWidget();
}

void UMainMenuWidget::OnAnimationCreateWidgetFinishedHandler()
{
	if (!CreateMatchWidget)
		return;

	if (!BackgroundBlur)
		return;

	if (!AnimShowCreateWidget)
		return;

	CreateMatchWidget->SetVisibility(ESlateVisibility::Hidden);
	BackgroundBlur->SetVisibility(ESlateVisibility::Hidden);
	UnbindAllFromAnimationFinished(AnimShowCreateWidget);
}

void UMainMenuWidget::OnAnimationFindWidgetFinishedHandler()
{
	if (!FindMatchWidget)
		return;

	if (!BackgroundBlur)
		return;

	if (!AnimShowFindWidget)
		return;

	FindMatchWidget->SetVisibility(ESlateVisibility::Hidden);
	BackgroundBlur->SetVisibility(ESlateVisibility::Hidden);
	UnbindAllFromAnimationFinished(AnimShowFindWidget);
}

void UMainMenuWidget::OnAnimationBlurFinishedHandler()
{
	if (!BackgroundBlur)
		return;

	BackgroundBlur->SetVisibility(ESlateVisibility::Hidden);
	UnbindAllFromAnimationFinished(AnimShowBackgroundBlur);
}

