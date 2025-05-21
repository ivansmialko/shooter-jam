// Made by smialko


#include "HUD/WorldChat.h"

#include "HUD/WorldChatItem.h"

#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UWorldChat::AddKillMessage(FString InAttackerName, FString InAttackedName)
{
	FString MessageText = FString::Printf(TEXT("%s eliminated %s"), *InAttackerName, *InAttackedName);
	AddMessage(MessageText);
}

void UWorldChat::AddMessage(const FString& InMessage)
{
	if (!Canvas)
		return;

	UWorldChatItem* NewItem = CreateWidget<UWorldChatItem>(GetOwningPlayer(), WorldChatItemBlueprint);
	if (!NewItem)
		return;

	NewItem->SetText(InMessage);

	UCanvasPanelSlot* NewSlot{ Canvas->AddChildToCanvas(NewItem) };
	if (!NewSlot)
		return;

	NewSlot->SetAnchors(FAnchors(0, 1, 0, 1));
	NewSlot->SetAlignment(FVector2D(0.f, 1.f));

	//UCanvasPanelSlot* CanvasSlot = Canvas->AddChildToCanvas(NewItem);
	//if (!CanvasSlot)
	//	return;

	for (const auto Message : MessagesHistory)
	{
		USizeBox* SizeBox{ Message->GetSizeBox() };
		if (!SizeBox)
			continue;

		UCanvasPanelSlot* MessageCanvasSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(Message) };
		if (!MessageCanvasSlot)
			continue;

		FVector2D Position{ MessageCanvasSlot->GetPosition() };
		FVector2D NewPosition(Position.X, Position.Y - SizeBox->GetDesiredSize().Y);
		MessageCanvasSlot->SetPosition(NewPosition);

	}

	FTimerHandle MessageTimerHandle;
	FTimerDelegate MessageTimerDelegate;
	MessageTimerDelegate.BindUFunction(this, FName("OnMessageTimerFinished"), NewItem);
	GetWorld()->GetTimerManager().SetTimer(MessageTimerHandle, MessageTimerDelegate, MessageDuration, false);

	MessagesHistory.Add(NewItem);
}

void UWorldChat::OnMessageTimerFinished(UWorldChatItem* InChatItem)
{
	if (!InChatItem)
		return;

	MessagesHistory.Remove(InChatItem);
	InChatItem->RemoveFromParent();
}
