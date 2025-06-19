// Made by smialko


#include "HUD/MainMenuFindMatchWidget.h"

#include "HUD/MainMenuFindMatchWidgetItem.h"

#include "Components/ScrollBox.h"

void UMainMenuFindMatchWidget::SetData(const FJoinWidgetData& InData)
{
	UpdateMatchesList(InData.MatchesList);
}

void UMainMenuFindMatchWidget::UpdateMatchesList(const TArray<FJoinWidgetItemData>& InMatchesList)
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

FJoinWidgetItemData::FJoinWidgetItemData(const FString& InMatchMode, const FString& InMatchName, const UINT32& InPing)
	:MatchMode(InMatchMode), MatchName(InMatchName), Ping(InPing)
{

}
