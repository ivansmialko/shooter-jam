// Made by smialko


#include "HUD/WorldChat.h"

#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"

void UWorldChat::AddKillMessage(FString InAttackerName, FString InAttackedName)
{
	APlayerController* PlayerController{ GetOwningPlayer() };
	if (!PlayerController)
		return;

	FString MessageText = FString::Printf(TEXT("%s eliminated %s"), *InAttackerName, *InAttackedName);
	AddMessage(MessageText);
}

void UWorldChat::AddMessage(const FString& InMessage)
{
	if (!Canvas)
		return;

	UTextBlock* NewTextBlock = NewObject<UTextBlock>(this);
	if (!NewTextBlock)
		return;

	NewTextBlock->SetText(FText::FromString(InMessage));

	UCanvasPanelSlot* CanvasSlot = Canvas->AddChildToCanvas(NewTextBlock);
	if (!CanvasSlot)
		return;

	//CanvasSlot->SetAnchors()
}
