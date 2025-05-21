// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldChat.generated.h"


class UCanvasPanel;
class UWorldChatItem;

UCLASS()
class SHOOTERJAM_API UWorldChat : public UUserWidget
{
	GENERATED_BODY()

//private members
private:

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* Canvas;

	TSubclassOf<UUserWidget> WorldChatItemBlueprint;

	TArray<UWorldChatItem*> MessagesHistory;
	float MessageDuration{ 3.f };

//public methods
public:
	void AddKillMessage(FString InAttackerName, FString InAttackedName);
	void AddMessage(const FString& InMessage);

//private methods
private:
	UFUNCTION()
	void OnMessageTimerFinished(UWorldChatItem* InMessage);


//public getters/setters
public:
	FORCEINLINE void SetMessageDuration(float InDuration) { MessageDuration = InDuration; }
	FORCEINLINE void SetChatItemBlueprint(const TSubclassOf<UUserWidget>& InBlueprint) { WorldChatItemBlueprint = InBlueprint; }
};
