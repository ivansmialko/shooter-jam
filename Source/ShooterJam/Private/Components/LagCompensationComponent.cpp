// Made by smialko


#include "Components/LagCompensationComponent.h"

#include "Characters/ShooterCharacter.h"

#include "Components/BoxComponent.h"

ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();
}


void ULagCompensationComponent::SaveFramePackage(FFramePackage& InPack)
{
	if (!Character)
		return;

	for (const auto& HitBox : Character->GetSsrCollisionBoxes())
	{
		FBoxInformation BoxInformation;
		BoxInformation.Location = HitBox.Value->GetComponentLocation();
		BoxInformation.Rotation = HitBox.Value->GetComponentRotation();
		BoxInformation.BoxExtent = HitBox.Value->GetScaledBoxExtent();
		InPack.HitBoxInfo.Add(BoxInformation);
	}
}

void ULagCompensationComponent::SetCharacter(AShooterCharacter* InCharacter)
{
	Character = InCharacter;
}

void ULagCompensationComponent::SetController(AShooterCharacterController* InController)
{
	Controller = InController;
}

void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

