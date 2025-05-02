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

void ULagCompensationComponent::ServerSideRewind(AShooterCharacter* InHitCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize& InHitLocation, float InHitTime)
{
	if (!InHitCharacter)
		return;

	ULagCompensationComponent* LagCompensation{ InHitCharacter->GetLagCompensationComponent() };
	if (!LagCompensation)
		return;

	const TDoubleLinkedList<FFramePackage>& History = LagCompensation->GetFrameHistory();
	if (!History.GetHead() || !History.GetTail())
		return;

	//If true - too far back, too laggy to perform SSR
	const float OldestHistoryTime{ History.GetTail()->GetValue().Time };
	if (OldestHistoryTime > InHitTime)
		return;

	//Frame package we check to verity the hit
	FFramePackage FrameToCheck;

	if (OldestHistoryTime == InHitTime)
	{
		FrameToCheck = History.GetTail()->GetValue();
	}

	const float NewestHistoryTime{ History.GetHead()->GetValue().Time };
	if (NewestHistoryTime <= InHitTime)
	{
		FrameToCheck = History.GetHead()->GetValue();
	}

	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Younger = History.GetHead();
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Older = Younger;
	while (Older->GetValue().Time > InHitTime) //is Older, still younger than hit time
	{
		if (!Older->GetNextNode())
			break;

		//March back until OlderTime < HitTime < YoungerTie
		Older = Older->GetNextNode();

		if (Older->GetValue().Time > InHitTime)
		{
			Younger = Older;
		}
	}
	if (Older->GetValue().Time == InHitTime) //highly unlikely, but we found our frame to check
	{
		FrameToCheck = Older->GetValue();
	}

	if (FrameToCheck.Time <= 1.f) //if FrameToCheck is uninitialized yet - use interpolation between Older and Younger
	{

	}
}

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& InPackage, FColor InColor)
{
	for (const auto& [HitBoxName, HitBoxInformation] : InPackage.HitBoxInfo)
	{
		DrawDebugBox(GetWorld(), HitBoxInformation.Location, HitBoxInformation.BoxExtent, FQuat(HitBoxInformation.Rotation), InColor, false, 4.f);
	}
}

