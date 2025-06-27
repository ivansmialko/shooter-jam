// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuCreateMatchWidget.generated.h"

class UComboBoxString;
class UEditableTextBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWidgetCreateMatchCreate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWidgetCreateMatchClose);

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

//public fields
public:
	UPROPERTY(BlueprintCallable)
	FWidgetCreateMatchCreate OnCreateDlg;
	UPROPERTY(BlueprintCallable)
	FWidgetCreateMatchCreate OnCloseDlg;

//private fields
private:
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ComboMatchType;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* FieldMatchName;

	FCreateWidgetData CreateWidgetData;
	
//public methods
public:
	void SetWidgetData(const FCreateWidgetData& InData);
	void GetUserData(FCreateWidgetUserData& OutUserData);

//private methods
private:
	void SetMatchName(const FString& InMatchName);
	void SetMatchTypes(const TArray<FString>& InMatchTypes);

};
