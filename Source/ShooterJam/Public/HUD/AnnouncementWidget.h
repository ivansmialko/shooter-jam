// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnnouncementWidget.generated.h"

class UTextBlock;

/**
 * C++ base for W_AnnouncementWidget. Used to display warmup time before the match starts
 */
UCLASS()
class SHOOTERJAM_API UAnnouncementWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WarmupTime;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AnnouncementText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoText;
};
