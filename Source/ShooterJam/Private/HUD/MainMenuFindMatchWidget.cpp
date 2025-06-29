// Made by smialko


#include "HUD/MainMenuFindMatchWidget.h"

#include "HUD/MainMenuFindMatchWidgetItem.h"

#include "Components/ScrollBox.h"

void UMainMenuFindMatchWidget::SetWidgetData(const FFindWidgetData& InData)
{
	UpdateMatchesList(InData.MatchesList);
}

bool UMainMenuFindMatchWidget::GetSessionId(FString& OutSessionId) const
{
	if (!CurrentItem)
		return false;

	OutSessionId = CurrentItem->GetSessionId();
	return true;
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
		NewItem->OnSelectedDlg.AddDynamic(this, &UMainMenuFindMatchWidget::OnItemSelected);
		MatchesList->AddChild(NewItem);
	}
}

void UMainMenuFindMatchWidget::OnItemSelected(UMainMenuFindMatchWidgetItem* InSelectedItem)
{
	if (!InSelectedItem)
		return;

	InSelectedItem->SetActive(true);

	if (CurrentItem)
	{
		CurrentItem->SetActive(false);
	}

	CurrentItem = InSelectedItem;
}
