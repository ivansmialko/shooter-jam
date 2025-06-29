// Made by smialko


#include "HUD/MainMenuFindMatchWidgetItem.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"

FFindWidgetItemData::FFindWidgetItemData(const FString& InMatchMode, const FString& InMatchName, const UINT32& InPing, const FString& InSessionId)
	:MatchMode(InMatchMode), MatchName(InMatchName), Ping(InPing), SessionId(InSessionId)
{

}

FFindWidgetItemData::FFindWidgetItemData() {}

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

	SessionId = InData.SessionId;
}

void UMainMenuFindMatchWidgetItem::SetActive(bool bInIsActive)
{
	if (!BackgroundActive)
		return;

	BackgroundActive->SetVisibility(bInIsActive ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

bool UMainMenuFindMatchWidgetItem::GetIsActive() const
{
	return (BackgroundActive && BackgroundActive->GetVisibility() == ESlateVisibility::Visible);
}
