// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuCreateMatchWidget.generated.h"

class UComboBoxString;
class UEditableTextBox;

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
	void SetMatchName(const FString& InMatchName);
	void SetMatchTypes(const TArray<FString>& InMatchTypes);
};
