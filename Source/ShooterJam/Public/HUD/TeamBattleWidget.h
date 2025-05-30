// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamBattleWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class SHOOTERJAM_API UTeamBattleWidget : public UUserWidget
{
	GENERATED_BODY()
	
//private members
private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBarRed;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBarBlue;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreRed;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreBlue;

	uint32 RedProgressMax{ 0 };
	uint32 BlueProgressMax{ 0 };

//public methods
public:
	void SetRedProgress(uint32 InProgress);
	void SetRedProgressMax(uint32 InMaxProgress);
	void SetBlueProgress(uint32 InProgress);
	void SetBlueProgressMax(uint32 InMaxProgress);
};
