// Made by smialko


#include "HUD/MainMenuFindMatchWidgetItem.h"

#include "Components/TextBlock.h"

FJoinWidgetItemData::FJoinWidgetItemData():
	MatchMode("none"), MatchName("none"), Ping(-1)
{

}

void UMainMenuFindMatchWidgetItem::SetData(const FJoinWidgetItemData& InData)
{
	if (TextMatchName)
	{
		TextMatchName->SetText(FText::FromString(InData.MatchName));
	}

	if (TextMatchType)
	{
		TextMatchType->SetText(FText::FromString(InData.MatchMode));
	}

	if (TextPing)
	{
		TextPing->SetText(FText::FromString(FString::FromInt(InData.Ping)));
	}
}
