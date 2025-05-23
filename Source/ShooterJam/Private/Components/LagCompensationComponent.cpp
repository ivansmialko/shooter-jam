// Made by smialko


#include "Components/LagCompensationComponent.h"

#include "Game/ShooterJam.h"
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
	FFramePackage FrameToCheck{ GetFrameToCheck(InHitCharacter, InHitTime) };
	return ConfirmHit(FrameToCheck, InHitCharacter, InTraceStart, InHitLocation);
}

FSsrResult ULagCompensationComponent::ServerSideRewindProjectile(AShooterCharacter* InHitCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize100& InInitialVelocity, float InHitTime)
{
	FFramePackage FrameToCheck{ GetFrameToCheck(InHitCharacter, InHitTime) };
	return ConfirmHitProjectile(FrameToCheck, InHitCharacter, InTraceStart, InInitialVelocity, InHitTime);
}

void ULagCompensationComponent::Server_ScoreRequest_Implementation(AShooterCharacter* InHitCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize& InHitLocation, float InHitTime)
{
	if (!InHitCharacter)
		return;

	if (!Character)
		return;

	AWeaponBase* EquippedWeapon{ Character->GetEquippedWeapon() };
	if (!EquippedWeapon)
		return;

	FSsrResult ConfirmResult = ServerSideRewind(InHitCharacter, InTraceStart, InHitLocation, InHitTime);
	if (!ConfirmResult.bHitConfirmed)
		return;

	float CurrentDamage{ ConfirmResult.bHeadshot ? EquippedWeapon->GetHeadDamage() : EquippedWeapon->GetBaseDamage() };
	UGameplayStatics::ApplyDamage(InHitCharacter, CurrentDamage, Character->GetController(), InHitCharacter, UDamageType::StaticClass());
}

void ULagCompensationComponent::Server_ScoreRequestProjectile_Implementation(AShooterCharacter* InHitCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize100& InitialVelocity, float InHitTime)
{
	if (!InHitCharacter)
		return;

	if (!Character)
		return;

	AWeaponBase* EquippedWeapon{ Character->GetEquippedWeapon() };
	if (!EquippedWeapon)
		return;

	FSsrResult ConfirmResult = ServerSideRewindProjectile(InHitCharacter, InTraceStart, InitialVelocity, InHitTime);

	if (!ConfirmResult.bHitConfirmed)
		return;

	float CurrentDamage{ ConfirmResult.bHeadshot ? EquippedWeapon->GetHeadDamage() : EquippedWeapon->GetBaseDamage() };
	UGameplayStatics::ApplyDamage(InHitCharacter, CurrentDamage, Character->GetController(), EquippedWeapon, UDamageType::StaticClass());
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

void ULagCompensationComponent::ResetPlayerBoxes(AShooterCharacter* InHitCharacter, const FFramePackage& InFramePackage, bool bInDisableCollision /*= false*/)
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

void ULagCompensationComponent::EnableHitBoxesCollisions(AShooterCharacter* InHitCharacter, ECollisionEnabled::Type InCollisionEnabled, ECollisionResponse InCollisionResponse)
{
	if (!InHitCharacter)
		return;

	//Confirm collision for the rest of the body
	for (const auto [HitBoxName, HitBoxComponent] : InHitCharacter->GetSsrCollisionBoxes())
	{
		if (!HitBoxComponent)
			continue;

		HitBoxComponent->SetCollisionEnabled(InCollisionEnabled);
		HitBoxComponent->SetCollisionResponseToChannel(ECC_HitBox, InCollisionResponse);
	}
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

FFramePackage ULagCompensationComponent::GetFrameToCheck(AShooterCharacter* InHitCharacter, float InHitTime)
{
	//Frame package we check to verity the hit
	FFramePackage FrameToCheck;

	if (!InHitCharacter)
		return FrameToCheck;

	ULagCompensationComponent* LagCompensation{ InHitCharacter->GetLagCompensationComponent() };
	if (!LagCompensation)
		return FrameToCheck;

	const TDoubleLinkedList<FFramePackage>& History = LagCompensation->GetFrameHistory();
	if (!History.GetHead() || !History.GetTail())
		return FrameToCheck;

	//If true - too far back, too laggy to perform SSR
	const float OldestHistoryTime{ History.GetTail()->GetValue().Time };
	if (OldestHistoryTime > InHitTime)
		return FrameToCheck;

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

	return FrameToCheck;
}

FSsrResult ULagCompensationComponent::ConfirmHit(const FFramePackage& InFramePackage, AShooterCharacter* InCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize& InHitLocation)
{
	FSsrResult ServerSideRewindResult;

	if (!InCharacter)
		return ServerSideRewindResult;

	UWorld* World{ GetWorld() };
	if (!World)
		return ServerSideRewindResult;

	FFramePackage CurrentFrame;
	GetFramePackage(CurrentFrame, InCharacter);		//Save current position of hit-boxes
	ResetPlayerBoxes(InCharacter, InFramePackage);	//Rewind hit-boxes positions to past
	EnablePlayerCollisions(InCharacter, ECollisionEnabled::QueryAndPhysics);
	EnableHitBoxesCollisions(InCharacter, ECollisionEnabled::QueryAndPhysics, ECollisionResponse::ECR_Block);
	
	FHitResult ConfirmShotResult;
	const FVector TraceEnd{ InTraceStart + (InHitLocation - InTraceStart) * 1.25f };

	World->LineTraceSingleByChannel(ConfirmShotResult, InTraceStart, TraceEnd, ECC_HitBox);
	if (ConfirmShotResult.bBlockingHit) //Collision confirmed. Also check if head shot
	{
		if (ConfirmShotResult.Component.IsValid())
		{
			UBoxComponent* Box{ Cast<UBoxComponent>(ConfirmShotResult.Component) };
			if (Box)
			{
				ServerSideRewindResult.bHeadshot = (Box->GetName() == "SSR_Head");
				DrawDebugBox(World, Box->GetComponentLocation(), Box->GetScaledBoxExtent(), FQuat(Box->GetComponentRotation()), (ServerSideRewindResult.bHeadshot ? FColor::Red : FColor::Blue), false, 8.f);
			}
		}

		ServerSideRewindResult.bHitConfirmed = true;
	}
	else //No collision confirmed
	{
		ServerSideRewindResult.bHitConfirmed = false;
		ServerSideRewindResult.bHeadshot = false;
	}

	ResetPlayerBoxes(InCharacter, CurrentFrame, true);
	EnablePlayerCollisions(InCharacter, ECollisionEnabled::QueryAndPhysics);
	return ServerSideRewindResult;
}

FSsrResult ULagCompensationComponent::ConfirmHitProjectile(const FFramePackage& InFramePackage, AShooterCharacter* InCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize100& InInitialVelocity, float InHitTime)
{
	FSsrResult ServerSideRewindResult;

	if (!InCharacter)
		return ServerSideRewindResult;

	UWorld* World{ GetWorld() };
	if (!World)
		return ServerSideRewindResult;

	FFramePackage CurrentFrame;
	GetFramePackage(CurrentFrame, InCharacter);		//Save current position of hit-boxes
	ResetPlayerBoxes(InCharacter, InFramePackage);	//Rewind hit-boxes positions to past
	EnablePlayerCollisions(InCharacter, ECollisionEnabled::QueryAndPhysics);
	EnableHitBoxesCollisions(InCharacter, ECollisionEnabled::QueryAndPhysics, ECollisionResponse::ECR_Block);

	FPredictProjectilePathParams PathParams;
	PathParams.bTraceWithCollision = true;
	PathParams.MaxSimTime = MaxRecordTime;
	PathParams.LaunchVelocity = InInitialVelocity;
	PathParams.StartLocation = InTraceStart;
	PathParams.SimFrequency = 15.f;
	PathParams.ProjectileRadius = 5.f;
	PathParams.TraceChannel = ECC_HitBox;
	PathParams.ActorsToIgnore.Add(GetOwner());
	PathParams.DrawDebugTime = 5.f;
	PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;

	FPredictProjectilePathResult PathResult;
	UGameplayStatics::PredictProjectilePath(World, PathParams, PathResult);
	if (PathResult.HitResult.bBlockingHit)
	{
		if (PathResult.HitResult.Component.IsValid())
		{
			UBoxComponent* Box{ Cast<UBoxComponent>(PathResult.HitResult.Component) };
			if (Box)
			{
				ServerSideRewindResult.bHeadshot = (Box->GetName() == "SSR_Head");
				DrawDebugBox(World, Box->GetComponentLocation(), Box->GetScaledBoxExtent(), FQuat(Box->GetComponentRotation()), (ServerSideRewindResult.bHeadshot ? FColor::Red : FColor::Blue), false, 8.f);
			}
		}

		ServerSideRewindResult.bHitConfirmed = true;
	}
	else
	{
		ServerSideRewindResult.bHitConfirmed = false;
		ServerSideRewindResult.bHeadshot = false;
	}

	ResetPlayerBoxes(InCharacter, CurrentFrame, true);
	EnablePlayerCollisions(InCharacter, ECollisionEnabled::QueryAndPhysics);
	return ServerSideRewindResult;
}

void ULagCompensationComponent::SetCharacter(AShooterCharacter* InCharacter)
{
	Character = InCharacter;
}

void ULagCompensationComponent::SetController(AShooterCharacterController* InController)
{
	Controller = InController;
}