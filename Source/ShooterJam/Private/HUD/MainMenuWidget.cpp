// Made by smialko


#include "HUD/MainMenuWidget.h"

#include "HUD/MainMenuCreateMatchWidget.h"

#include "Components/BackgroundBlur.h"
#include "Components/Widget.h"
#include "Animation/WidgetAnimation.h"
void UMainMenuWidget::MenuSetup()
{

}

void UMainMenuWidget::ShowCreateWidget(const FCreateWidgetData& InData)
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

	CreateMatchWidget->SetMatchTypes(InData.MatchModesList);
	CreateMatchWidget->SetMatchName(InData.LastMatchName);
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

void UMainMenuWidget::ShowJoinWidget(const FJoinWidgetData& InData)
{
}

void UMainMenuWidget::OnAnimationCreateWidgetFinishedHandler()
{
	if (!CreateMatchWidget)
		return;

	CreateMatchWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UMainMenuWidget::OnAnimationBlurFinishedHandler()
{
	if (!BackgroundBlur)
		return;

	BackgroundBlur->SetVisibility(ESlateVisibility::Hidden);
}

