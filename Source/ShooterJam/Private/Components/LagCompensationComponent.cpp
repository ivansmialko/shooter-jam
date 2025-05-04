// Made by smialko


#include "Components/LagCompensationComponent.h"

#include "Characters/ShooterCharacter.h"
#include "Weaponry/WeaponBase.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SaveFrame();
}

FSsrResult ULagCompensationComponent::ServerSideRewind(AShooterCharacter* InHitCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize& InHitLocation, float InHitTime)
{
	if (!InHitCharacter)
		return FSsrResult();

	ULagCompensationComponent* LagCompensation{ InHitCharacter->GetLagCompensationComponent() };
	if (!LagCompensation)
		return FSsrResult();

	const TDoubleLinkedList<FFramePackage>& History = LagCompensation->GetFrameHistory();
	if (!History.GetHead() || !History.GetTail())
		return FSsrResult();

	//If true - too far back, too laggy to perform SSR
	const float OldestHistoryTime{ History.GetTail()->GetValue().Time };
	if (OldestHistoryTime > InHitTime)
		return FSsrResult();

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
		FrameToCheck = InterpolateBetweenFrames(Older->GetValue(), Younger->GetValue(), InHitTime);
	}

	return ConfirmHit(FrameToCheck, InHitCharacter, InTraceStart, InHitLocation);
}

void ULagCompensationComponent::Server_ScoreRequest_Implementation(AShooterCharacter* InHitCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize& InHitLocation, float InHitTime, AWeaponBase* InWeapon)
{
	FSsrResult ConfirmResult = ServerSideRewind(InHitCharacter, InTraceStart, InHitLocation, InHitTime);

	if (InHitCharacter && ConfirmResult.bHitConfirmed && Character)
	{
		UGameplayStatics::ApplyDamage(InHitCharacter, InWeapon->GetBaseDamage(), Character->GetController(), InHitCharacter, UDamageType::StaticClass());
	}
}

void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& InPackage, FColor InColor)
{
	for (const auto& [HitBoxName, HitBoxInformation] : InPackage.HitBoxInfo)
	{
		DrawDebugBox(GetWorld(), HitBoxInformation.Location, HitBoxInformation.BoxExtent, FQuat(HitBoxInformation.Rotation), InColor, false, 4.f);
	}
}

void ULagCompensationComponent::GetFramePackage(FFramePackage& InPack, AShooterCharacter* InCharacter /*= nullptr*/)
{
	AShooterCharacter* CharacterToUse{ InCharacter };
	if (!CharacterToUse)
	{
		CharacterToUse = Character;
	}

	if (!CharacterToUse)
		return;

	for (const auto& [HitBoxName, HitBoxComponent] : CharacterToUse->GetSsrCollisionBoxes())
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
	if (!Character)
		return;

	if (!Character->HasAuthority())
		return;

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

		//ShowFramePackage(ThisFrame, FColor::Red);
	}
}

void ULagCompensationComponent::RewindPlayerBoxes(AShooterCharacter* InHitCharacter, const FFramePackage& InFramePackage, bool bInDisableCollision /*= false*/)
{
	if (!InHitCharacter)
		return;

	for (const auto& [HitBoxName, HitBoxComponent] : InHitCharacter->GetSsrCollisionBoxes())
	{
		if (!HitBoxComponent)
			continue;

		auto HitBox { InFramePackage.HitBoxInfo.Find(HitBoxName) };
		if (!HitBox)
			continue;

		HitBoxComponent->SetWorldLocation(HitBox->Location);
		HitBoxComponent->SetWorldRotation(HitBox->Rotation);
		HitBoxComponent->SetBoxExtent(HitBox->BoxExtent);
		if (bInDisableCollision)
		{
			HitBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

}

void ULagCompensationComponent::EnablePlayerCollisions(AShooterCharacter* InHitCharacter, ECollisionEnabled::Type InCollisionEnabled)
{
	if (!InHitCharacter)
		return;

	USkeletalMeshComponent* CharacterMesh{ InHitCharacter->GetMesh() };
	if (!CharacterMesh)
		return;

	CharacterMesh->SetCollisionEnabled(InCollisionEnabled);
}

FFramePackage ULagCompensationComponent::InterpolateBetweenFrames(const FFramePackage& InOlderFrame, const FFramePackage& InYoungerFrame, float InHitTime)
{
	const float Distance{ InYoungerFrame.Time - InOlderFrame.Time };
	const float InterpFraction{ FMath::Clamp((InHitTime - InOlderFrame.Time) / Distance, 0.f, 1.f) };

	FFramePackage InterpFramePackage;
	InterpFramePackage.Time = InHitTime;

	for (const auto& [YoungerName, YoungerBoxInfo] : InYoungerFrame.HitBoxInfo)
	{
		const FBoxInformation& OlderBoxInfo = InOlderFrame.HitBoxInfo[YoungerName];

		FBoxInformation InterpBoxInfo;
		InterpBoxInfo.Location = FMath::VInterpTo(OlderBoxInfo.Location, YoungerBoxInfo.Location, 1.f, InterpFraction);
		InterpBoxInfo.Rotation = FMath::RInterpTo(OlderBoxInfo.Rotation, YoungerBoxInfo.Rotation, 1.f, InterpFraction);
		InterpBoxInfo.BoxExtent = YoungerBoxInfo.BoxExtent;

		InterpFramePackage.HitBoxInfo.Add(YoungerName, InterpBoxInfo);
	}

	return InterpFramePackage;
}

FSsrResult ULagCompensationComponent::ConfirmHit(const FFramePackage& InFramePackage, AShooterCharacter* InCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize& InHitLocation)
{
	if (!InCharacter)
		return FSsrResult();

	FFramePackage CurrentFrame;
	GetFramePackage(CurrentFrame, InCharacter);
	RewindPlayerBoxes(InCharacter, InFramePackage);
	EnablePlayerCollisions(InCharacter, ECollisionEnabled::NoCollision);

	FHitResult ConfirmShotResult;
	const FVector TraceEnd{ InTraceStart + (InHitLocation - InTraceStart) * 1.25f };

	// Confirm collision for the headshot first
	UBoxComponent** HeadBox{ InCharacter->GetSsrCollisionBoxes().Find(FName("head")) };
	if (!HeadBox || !(*HeadBox))
	{
		(*HeadBox)->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		(*HeadBox)->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

		if (UWorld* World{ GetWorld() })
		{
			World->LineTraceSingleByChannel(ConfirmShotResult, InTraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
			if (ConfirmShotResult.bBlockingHit)
			{
				RewindPlayerBoxes(InCharacter, CurrentFrame, true);
				EnablePlayerCollisions(InCharacter, ECollisionEnabled::QueryAndPhysics);
				return FSsrResult{ true, true };
			}
		}
	}

	//Confirm collision for the rest of the body
	for (const auto [HitBoxName, HitBoxComponent] : InCharacter->GetSsrCollisionBoxes())
	{
		if (!HitBoxComponent)
			continue;

		HitBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		HitBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	}

	if (UWorld * World{ GetWorld() })
	{
		World->LineTraceSingleByChannel(ConfirmShotResult, InTraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
		if (ConfirmShotResult.bBlockingHit)
		{
			RewindPlayerBoxes(InCharacter, CurrentFrame, true);
			EnablePlayerCollisions(InCharacter, ECollisionEnabled::QueryAndPhysics);
			return FSsrResult{ true, true };
		}
	}

	//No collision confirmed
	RewindPlayerBoxes(InCharacter, CurrentFrame, true);
	EnablePlayerCollisions(InCharacter, ECollisionEnabled::QueryAndPhysics);
	return FSsrResult{ false, false };
}

void ULagCompensationComponent::SetCharacter(AShooterCharacter* InCharacter)
{
	Character = InCharacter;
}

void ULagCompensationComponent::SetController(AShooterCharacterController* InController)
{
	Controller = InController;
}