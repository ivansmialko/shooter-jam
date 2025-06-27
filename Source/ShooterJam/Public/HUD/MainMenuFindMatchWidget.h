// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuFindMatchWidget.generated.h"

class UScrollBox;
class UMainMenuFindMatchWidgetItem;
struct FFindWidgetItemData;

USTRUCT()
struct FFindWidgetData
{
	GENERATED_BODY()
	TArray<FFindWidgetItemData> MatchesList;
};

UCLASS()
class SHOOTERJAM_API UMainMenuFindMatchWidget : public UUserWidget
{
	GENERATED_BODY()

//private fields
private:
	UPROPERTY(meta = (BindWidget))
	UScrollBox* MatchesList;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMainMenuFindMatchWidgetItem> ListItemBlueprint;

//public methods
public:
	void SetWidgetData(const FFindWidgetData& InData);

//private methods
private:
	void UpdateMatchesList(const TArray<FFindWidgetItemData>& InMatchesList);
};
