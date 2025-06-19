// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuFindMatchWidgetItem.generated.h"

class UTextBlock;

USTRUCT()
struct FJoinWidgetItemData
{
	GENERATED_BODY()

	FJoinWidgetItemData();
	FJoinWidgetItemData(const FString& InMatchMode, const FString& InMatchName, const UINT32& InPing);

	FString MatchMode;
	FString MatchName;
	uint32 Ping;
};

UCLASS()
class SHOOTERJAM_API UMainMenuFindMatchWidgetItem : public UUserWidget
{
	GENERATED_BODY()

//private fields
private:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextMatchName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextMatchType;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextPing;

public:
	void SetData(const FJoinWidgetItemData& InData);
};
