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


void ULagCompensationComponent::GetFramePackage(FFramePackage& InPack)
{
	if (!Character)
		return;

	for (const auto& [HitBoxName, HitBoxComponent] : Character->GetSsrCollisionBoxes())
	{
		FBoxInformation BoxInformation;
		BoxInformation.Location = HitBoxComponent->GetComponentLocation();
		BoxInformation.Rotation = HitBoxComponent->GetComponentRotation();
		BoxInformation.BoxExtent = HitBoxComponent->GetScaledBoxExtent();
		InPack.HitBoxInfo.Add(HitBoxName, BoxInformation);
	}

	InPack.Time = GetWorld()->GetTimeSeconds();
}

void ULagCompensationComponent::SaveFrame()
{
	if (FrameHistory.Num() <= 1)
	{
		FFramePackage ThisFrame;
		GetFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
	}
	else
	{
		float HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		while (HistoryLength > MaxRecordTime)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		}

		FFramePackage ThisFrame;
		GetFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);

		ShowFramePackage(ThisFrame, FColor::Red);
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

	SaveFrame();
}

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& InPackage, FColor InColor)
{
	for (const auto& [HitBoxName, HitBoxInformation] : InPackage.HitBoxInfo)
	{
		DrawDebugBox(GetWorld(), HitBoxInformation.Location, HitBoxInformation.BoxExtent, FQuat(HitBoxInformation.Rotation), InColor, false, 4.f);
	}
}

