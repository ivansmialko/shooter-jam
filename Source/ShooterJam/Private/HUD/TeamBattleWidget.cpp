// Made by smialko


#include "HUD/TeamBattleWidget.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UTeamBattleWidget::SetRedProgress(uint32 InProgress)
{
	ScoreRed->SetText(FText::FromString(FString::Printf("%d", InProgress)));

	float ProgressPercent{ (InProgress == 0 ? InProgress : (RedProgressMax / InProgress)) };
	ProgressBarRed->SetPercent(ProgressPercent);
}

void UTeamBattleWidget::SetRedProgressMax(uint32 InMaxProgress)
{
	RedProgressMax = InMaxProgress;
}

void UTeamBattleWidget::SetBlueProgress(uint32 InProgress)
{
	ScoreBlue->SetText(FText::FromString(FString::Printf("%d", InProgress)));

	float ProgressPercent{ (InProgress == 0 ? InProgress : (RedProgressMax / InProgress)) };
	ProgressBarBlue->SetPercent(ProgressPercent);
}

void UTeamBattleWidget::SetBlueProgressMax(uint32 InMaxProgress)
{
	BlueProgressMax = InMaxProgress;
}
