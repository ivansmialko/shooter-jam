// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuFindMatchWidgetItem.generated.h"

class UTextBlock;
class UImage;

USTRUCT()
struct FFindWidgetItemData
{
	GENERATED_BODY()

	FFindWidgetItemData();
	FFindWidgetItemData(const FString& InMatchMode, const FString& InMatchName, const UINT32& InPing, const FString& SessionId);

	FString MatchMode;
	FString MatchName;
	FString SessionId;
	uint32 Ping;
};

UCLASS()
class SHOOTERJAM_API UMainMenuFindMatchWidgetItem : public UUserWidget
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWidgetFindMatchItemSelect, UMainMenuFindMatchWidgetItem*, SelectedItem);

//public fields
public:
	UPROPERTY(BlueprintCallable)
	FWidgetFindMatchItemSelect OnSelectedDlg;

//private fields
private:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextMatchName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextMatchType;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextPing;

	UPROPERTY(meta = (BindWidget))
	UImage* BackgroundActive;

	FString SessionId;

public:
	void SetData(const FFindWidgetItemData& InData);
	void SetActive(bool bInIsActive);

	UFUNCTION(BlueprintCallable)
	bool GetIsActive() const;

	FORCEINLINE FString GetSessionId() const { return SessionId; }
};
