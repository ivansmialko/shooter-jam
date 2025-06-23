// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuCreateMatchWidget.generated.h"

class UComboBoxString;
class UEditableTextBox;


USTRUCT()
struct FCreateWidgetData
{
	GENERATED_BODY()

	FString LastMatchName;
	TArray<FString> MatchModesList;
};

USTRUCT()
struct FCreateWidgetUserData
{
	GENERATED_BODY()

	FString MatchName;
	FString MatchMode;
	uint32 MaxPlayers{ 0 };
};

UCLASS()
class SHOOTERJAM_API UMainMenuCreateMatchWidget : public UUserWidget
{
	GENERATED_BODY()

//private fields
private:

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ComboMatchType;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* FieldMatchName;

//public methods
public:
	void SetData(const FCreateWidgetData& InData);
	void GetUserData(FCreateWidgetUserData& OutUserData);

private:
	void SetMatchName(const FString& InMatchName);
	void SetMatchTypes(const TArray<FString>& InMatchTypes);
};
