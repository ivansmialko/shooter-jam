// Made by smialko


#include "HUD/MainMenuFindMatchWidget.h"

#include "HUD/MainMenuFindMatchWidgetItem.h"

#include "Components/ScrollBox.h"

void UMainMenuFindMatchWidget::SetWidgetData(const FFindWidgetData& InData)
{
	UpdateMatchesList(InData.MatchesList);
}

FString UMainMenuFindMatchWidget::GetSessionId()
{
	for (uint32 i = 0; i < MatchesList->GetChildrenCount(); ++i)
	{
		UMainMenuFindMatchWidgetItem* ListItem = Cast<UMainMenuFindMatchWidgetItem>(MatchesList->GetChildAt(i));
		if(!ListItem)
			continue;


	}
}

void UMainMenuFindMatchWidget::UpdateMatchesList(const TArray<FFindWidgetItemData>& InMatchesList)
{
	if (!ListItemBlueprint)
		return;

	APlayerController* OwningController = GetWorld()->GetFirstPlayerController();
	if (!OwningController)
		return;

	MatchesList->ClearChildren();

	for (const auto& MatchData : InMatchesList)
	{
		UMainMenuFindMatchWidgetItem* NewItem = CreateWidget<UMainMenuFindMatchWidgetItem>(OwningController, ListItemBlueprint);
		if(!NewItem)
			continue;

		NewItem->SetData(MatchData);
		MatchesList->AddChild(NewItem);
	}
}
