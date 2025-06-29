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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWidgetFindMatchClose);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWidgetFindMatchJoin);

//public fields
public:
	UPROPERTY(BlueprintCallable)
	FWidgetFindMatchClose OnCloseDlg;

	UPROPERTY(BlueprintCallable)
	FWidgetFindMatchJoin OnJoinDlg;

//private fields
private:
	UPROPERTY(meta = (BindWidget))
	UScrollBox* MatchesList;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMainMenuFindMatchWidgetItem> ListItemBlueprint;

	UMainMenuFindMatchWidgetItem* CurrentItem{ nullptr };

//public methods
public:
	void SetWidgetData(const FFindWidgetData& InData);
	bool GetSessionId(FString& OutSessionId) const;

//private methods
private:
	void UpdateMatchesList(const TArray<FFindWidgetItemData>& InMatchesList);

	UFUNCTION()
	void OnItemSelected(UMainMenuFindMatchWidgetItem* InSelectedItem);
};
