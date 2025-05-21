// Made by smialko


#include "HUD/WorldChatItem.h"

#include "Components/TextBlock.h"

void UWorldChatItem::SetText(const FString& InText)
{
	if (!MessageText)
		return;

	MessageText->SetText(FText::FromString(InText));
}
