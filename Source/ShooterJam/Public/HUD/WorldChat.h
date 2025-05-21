// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldChat.generated.h"


class UCanvasPanel;

UCLASS()
class SHOOTERJAM_API UWorldChat : public UUserWidget
{
	GENERATED_BODY()

//private members
private:

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* Canvas;

//public methods
public:
	void AddKillMessage(FString InAttackerName, FString InAttackedName);
	void AddMessage(const FString& InMessage);

//public getters/setters
public:
	//void SetMessageDuration(float InDuration);
};
