// Made by smialko

#include "Components/CombatComponent.h"

#include "Weaponry/WeaponBase.h"
#include "PlayerControllers/ShooterCharacterController.h"
#include "ShooterCharacter.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"

UCombatComponent::UCombatComponent()
{
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!Character)
		return;

	if (Character->GetFollowCamera())
	{
		FovDefault = Character->GetFollowCamera()->FieldOfView;
		FovCurrent = FovDefault;
	}
}


void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (!Character)
		return;

	if (!Character->GetController())
		return;

	if (!CharacterController)
	{
		CharacterController = Cast<AShooterCharacterController>(Character->GetController());
	}

	if (!CharacterController)
		return;

	if (!CharacterController->GetHUD())
		return;

	if (!ShooterHud)
	{
		ShooterHud = Cast<AShooterHUD>(CharacterController->GetHUD());
	}

	if (!ShooterHud)
		return;

	if (!EquippedWeapon)
	{
		FHUDPackage EmptyHudPackage;
		ShooterHud->SetHUDPackage(EmptyHudPackage);
		return;
	}

	HudPackage.CrosshairsCenter = EquippedWeapon->GetCrosshairsCenter();
	HudPackage.CrosshairsLeft = EquippedWeapon->GetCrosshairsLeft();
	HudPackage.CrosshairsRight = EquippedWeapon->GetCrosshairsRight();
	HudPackage.CrosshairsBottom = EquippedWeapon->GetCrosshairsBottom();
	HudPackage.CrosshairsTop = EquippedWeapon->GetCrosshairsTop();

	//Calculate spread
	FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
	FVector2D VelocityRange(0.f, 1.f);
	
	FVector Velocity{ Character->GetVelocity() };
	Velocity.Z = 0;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityRange, Velocity.Size());

	if (Character->GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	if (bIsAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 30.f);

	HudPackage.CrosshairSpread = 0.5f;
	HudPackage.CrosshairSpread += CrosshairVelocityFactor;
	HudPackage.CrosshairSpread += CrosshairInAirFactor;
	HudPackage.CrosshairSpread -= CrosshairAimFactor;
	HudPackage.CrosshairSpread += CrosshairShootingFactor;

	ShooterHud->SetHUDPackage(HudPackage);
}

void UCombatComponent::InterpFov(float DeltaTime)
{
	if (!EquippedWeapon)
		return;

	if (!Character)
		return;

	UCameraComponent* FollowCamera = Character->GetFollowCamera();
	if (!FollowCamera)
		return;

	if (bIsAiming)
	{
		FovCurrent = FMath::FInterpTo(FovCurrent, EquippedWeapon->GetFovZoomed(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		FovCurrent = FMath::FInterpTo(FovCurrent, FovDefault, DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}


	FollowCamera->SetFieldOfView(FovCurrent);
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (Character && Character->IsLocallyControlled())
	{
		SetHUDCrosshairs(DeltaTime);

		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		InterpFov(DeltaTime);
	}
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
}

void UCombatComponent::EquipWeapon(class AWeaponBase* InWeaponToEquip)
{
	if (!Character)
		return;

	if (!InWeaponToEquip)
		return;

	EquippedWeapon = InWeaponToEquip;

	const USkeletalMeshSocket* HandSocket{ Character->GetMesh()->GetSocketByName(FName("RightHandSocket")) };
	if (!HandSocket)
		return;

	HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	EquippedWeapon->ChangeWeaponState(EWeaponState::EWS_Equipped);
	EquippedWeapon->SetOwner(Character);
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

bool UCombatComponent::GetIsWeaponEquipped()
{
	return (EquippedWeapon != nullptr);
}

bool UCombatComponent::GetIsAiming()
{
	return bIsAiming;
}

AWeaponBase* UCombatComponent::GetEquippedWeapon() const
{
	return EquippedWeapon;
}

FVector UCombatComponent::GetHitTarget() const
{
	return HitTarget;
}

void UCombatComponent::SetIsAiming(bool bInIsAiming)
{
	bIsAiming = bInIsAiming;
}

void UCombatComponent::SetIsFiring(bool bInIsFiring)
{
	bIsFiring = bInIsFiring;

	if (!bInIsFiring)
		return;

	if (!Character)
		return;

	Character->PlayFireMontage(bIsAiming);

	if (!EquippedWeapon)
		return;

	EquippedWeapon->Fire(HitTarget);

	CrosshairShootingFactor = 0.75f;
}

void UCombatComponent::SetHitTarget(const FVector& TraceHitTarget)
{
	HitTarget = TraceHitTarget;
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Received replication"));

	if (!EquippedWeapon)
		return;

	UE_LOG(LogTemp, Warning, TEXT("Weapon is ok"));

	if (!Character)
		return;

	UE_LOG(LogTemp, Warning, TEXT("Character is ok"));

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	if (!GEngine)
		return;

	if (!GEngine->GameViewport)
		return;

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	APlayerController* CurrentPlayerController{ UGameplayStatics::GetPlayerController(this, 0) };
	if (!CurrentPlayerController)
		return;

	FVector CrosshairsWorldPosition;
	FVector CrosshairsWorldDirection;
	FVector2D CrosshairsLocation{ ViewportSize.X * 0.5f, ViewportSize.Y * 0.5f };

	//Find world coordinates of screen-space coordinate
	bool bIsSuccessfull = UGameplayStatics::DeprojectScreenToWorld(CurrentPlayerController, CrosshairsLocation, CrosshairsWorldPosition, CrosshairsWorldDirection);
	if (!bIsSuccessfull)
		return;

	const float LinetraceLength{ 80000.f };
	FVector LinetraceStart{ CrosshairsWorldPosition };
	FVector LinetraceEnd{ LinetraceStart + CrosshairsWorldDirection * LinetraceLength };

	GetWorld()->LineTraceSingleByChannel(
		TraceHitResult,
		LinetraceStart,
		LinetraceEnd,
		ECollisionChannel::ECC_Visibility);

	if (!TraceHitResult.bBlockingHit)
	{
		TraceHitResult.ImpactPoint = LinetraceEnd;
		return;
	}

	if (TraceHitResult.GetActor()
		&& TraceHitResult.GetActor()->Implements<UCrosshairsInteractable>())
	{
		HudPackage.CrosshairsColor = FLinearColor::Red;
	}
	else
	{
		HudPackage.CrosshairsColor = FLinearColor::White;
	}

	////If nothing is hit - set position to linetrace end
	//if (!TraceHitResult.bBlockingHit)
	//{
	//	TraceHitResult.ImpactPoint = LinetraceEnd;
	//}
	//else
	//{
	//	DrawDebugSphere(
	//		GetWorld(),
	//		TraceHitResult.ImpactPoint,
	//		12.f,
	//		12,
	//		FColor::Red);
	//}

	//HitTarget = TraceHitResult.ImpactPoint;
}

