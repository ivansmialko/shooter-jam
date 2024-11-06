// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverheadWidget.h"
#include "Components/TextBlock.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (!DisplayText)
		return;

	DisplayText->SetText(FText::FromString(TextToDisplay));

}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole LocalRole = InPawn->GetLocalRole();
	FString LocalRoleText;
	switch (LocalRole)
	{
	case ROLE_SimulatedProxy:
	{
		LocalRoleText = FString("SimulatedProxy");
	} break;
	case ROLE_AutonomousProxy:
	{
		LocalRoleText = FString("AutonomousProxy");
	} break;
	case ROLE_Authority:
	{
		LocalRoleText = FString("Authority");
	} break;
	default:
	{
		LocalRoleText = FString("None");
	} break;
	}

	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString RemoteRoleText;
	switch (RemoteRole)
	{
	case ROLE_SimulatedProxy:
	{
		RemoteRoleText = FString("SimulatedProxy");
	} break;
	case ROLE_AutonomousProxy:
	{
		RemoteRoleText = FString("AutonomousProxy");
	} break;
	case ROLE_Authority:
	{
		RemoteRoleText = FString("Authority");
	} break;
	default:
	{
		RemoteRoleText = FString("None");
	} break;
	}

	FString WidgetText = FString::Printf(TEXT("Local Role %s\nRemote role %s"), *LocalRoleText, *RemoteRoleText);
	SetDisplayText(WidgetText);
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
