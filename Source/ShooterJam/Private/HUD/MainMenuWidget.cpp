// Made by smialko


#include "HUD/MainMenuWidget.h"

#include "HUD/MainMenuCreateMatchWidget.h"
#include "HUD/MainMenuFindMatchWidget.h"

#include "Components/BackgroundBlur.h"
#include "Components/Widget.h"
#include "Animation/WidgetAnimation.h"

void UMainMenuWidget::SetCreateParams(const FCreateWidgetData& InCreateParams)
{
	if (!CreateMatchWidget)
		return;

	CreateMatchWidget->SetWidgetData(InCreateParams);
}

void UMainMenuWidget::MenuSetup()
{

}

void UMainMenuWidget::InitializeListeners()
{
	if (CreateMatchWidget)
	{
		//CreateMatchWidget->OnCreateDlg.
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

//void UMainMenuWidget::ShowJoinWidget(const FJoinWidgetData& InData)
//{
//	if (!FindMatchWidget)
//		return;
//
//	if (!AnimShowFindWidget)
//		return;
//
//	if (!AnimShowBackgroundBlur)
//		return;
//
//	FindMatchWidget->SetVisibility(ESlateVisibility::Visible);
//	BackgroundBlur->SetVisibility(ESlateVisibility::Visible);
//
//	PlayAnimation(AnimShowFindWidget);
//	PlayAnimation(AnimShowBackgroundBlur);
//
//	FindMatchWidget->SetData(InData);
//}

//void UMainMenuWidget::HideJoinWidget()
//{
//	if (!AnimShowBackgroundBlur)
//		return;
//
//	if (!AnimShowFindWidget)
//		return;
//
//	PlayAnimation(AnimShowBackgroundBlur, 0.f, 1, EUMGSequencePlayMode::Reverse);
//	PlayAnimation(AnimShowFindWidget, 0.f, 1, EUMGSequencePlayMode::Reverse);
//
//	FWidgetAnimationDynamicEvent OnAnimationBlurFinished;
//	OnAnimationBlurFinished.BindDynamic(this, &UMainMenuWidget::OnAnimationBlurFinishedHandler);
//	FWidgetAnimationDynamicEvent OnAnimationFindWidgetFinished;
//	OnAnimationFindWidgetFinished.BindDynamic(this, &UMainMenuWidget::OnAnimationFindWidgetFinishedHandler);
//
//	BindToAnimationFinished(AnimShowBackgroundBlur, OnAnimationBlurFinished);
//	BindToAnimationFinished(AnimShowFindWidget, OnAnimationFindWidgetFinished);
//}

void UMainMenuWidget::OnCreateMatchCreate(FCreateWidgetUserData InUserData)
{
	OnCreateMatchDlg.Execute(InUserData);
}

void UMainMenuWidget::OnCreateMatchClose()
{
	HideCreateWidget();
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

//void UMainMenuWidget::OnAnimationFindWidgetFinishedHandler()
//{
//	if (!FindMatchWidget)
//		return;
//
//	if (!BackgroundBlur)
//		return;
//
//	if (!AnimShowFindWidget)
//		return;
//
//	FindMatchWidget->SetVisibility(ESlateVisibility::Hidden);
//	BackgroundBlur->SetVisibility(ESlateVisibility::Hidden);
//	UnbindAllFromAnimationFinished(AnimShowFindWidget);
//}

void UMainMenuWidget::OnAnimationBlurFinishedHandler()
{
	if (!BackgroundBlur)
		return;

	BackgroundBlur->SetVisibility(ESlateVisibility::Hidden);
	UnbindAllFromAnimationFinished(AnimShowBackgroundBlur);
}

