// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldChatItem.generated.h"

class UTextBlock;
class USizeBox;

UCLASS()
class SHOOTERJAM_API UWorldChatItem : public UUserWidget
{
	GENERATED_BODY()
	
//private methods
private:

	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MessageText;

//public methods
public:
	void SetText(const FString& InText);


//public getters/setters
public:
	FORCEINLINE USizeBox* GetSizeBox() const { return SizeBox; }
	FORCEINLINE UTextBlock* GetTextBlock() const { return MessageText; }
};
