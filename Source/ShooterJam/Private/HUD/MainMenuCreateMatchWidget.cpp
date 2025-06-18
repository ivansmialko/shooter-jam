// Made by smialko


#include "HUD/MainMenuCreateMatchWidget.h"

#include "Components/EditableTextBox.h"
#include "Components/ComboBoxString.h"

void UMainMenuCreateMatchWidget::SetMatchName(const FString& InMatchName)
{
	if (!FieldMatchName)
		return;

	FieldMatchName->SetText(FText::FromString(InMatchName));
}

void UMainMenuCreateMatchWidget::SetMatchTypes(const TArray<FString>& InMatchTypes)
{
	ComboMatchType->ClearOptions();
	for (const auto& TypeString : InMatchTypes)
	{
		ComboMatchType->AddOption(TypeString);
	}

	if (ComboMatchType->GetOptionCount() > 0)
	{
		ComboMatchType->SetSelectedIndex(0);
	}
}
