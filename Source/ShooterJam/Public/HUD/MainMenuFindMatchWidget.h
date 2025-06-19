// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuFindMatchWidget.generated.h"

class UScrollBox;

UCLASS()
class SHOOTERJAM_API UMainMenuFindMatchWidget : public UUserWidget
{
	GENERATED_BODY()

//private fields
private:
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ScrollBox;


//public methods
public:
	void 
};
