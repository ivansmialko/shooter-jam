// Made by smialko


#include "HUD/MainMenuFindMatchWidgetItem.h"

#include "Components/TextBlock.h"

FFindWidgetItemData::FFindWidgetItemData():
	MatchMode("none"), MatchName("none"), Ping(-1)
{

}

void UMainMenuFindMatchWidgetItem::SetData(const FFindWidgetItemData& InData)
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
