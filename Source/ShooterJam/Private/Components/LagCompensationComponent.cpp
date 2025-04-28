// Made by smialko


#include "Components/LagCompensationComponent.h"

ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();
}


{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

