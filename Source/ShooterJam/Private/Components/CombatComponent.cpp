// Made by smialko

#include "Components/CombatComponent.h"

#include "Weaponry/WeaponBase.h"
#include "PlayerControllers/ShooterCharacterController.h"
#include "Characters/ShooterCharacter.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"

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

	if (Character->HasAuthority())
	{
		InitializeCarriedAmmo();
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
		FCrosshairsPackage EmptyHudPackage;
		ShooterHud->SetCrosshairsPackage(EmptyHudPackage);
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

	ShooterHud->SetCrosshairsPackage(HudPackage);
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

void UCombatComponent::StartFireTimer()
{
	if (!EquippedWeapon)
		return;

	if (!Character)
		return;

	FTimerManager& WorldTimerManager{ Character->GetWorldTimerManager() };
	WorldTimerManager.SetTimer(FireTimerHandler, this, &UCombatComponent::OnFireTimerFinished, EquippedWeapon->GetFireDelay());
}

void UCombatComponent::OnFireTimerFinished()
{
	if (!EquippedWeapon)
		return;

	bIsCanFire = true;

	if (!bIsFiring)
		return;

	if (!EquippedWeapon->GetIsAutomatic())
		return;

	//If firing button is still pressed, and weapon is automatic - shoot again automatically
	FireWeapon();

	if (!EquippedWeapon->IsEmpty())
		return;

	//Reload weapon automatically, if firing button is still pressed
	ReloadWeapon();
}

void UCombatComponent::OnReloadFinished()
{
	if (!Character)
		return;

	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
		ReloadAmmo();

		//Resume firing. Only aplicable for server.
		//For the client, OnRep_CombatState is the case
		if (bIsFiring)
		{
			FireWeapon();
		}
	}
}

void UCombatComponent::OnThrowFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("Finished anim"));

	CombatState = ECombatState::ECS_Unoccupied;
}

void UCombatComponent::FireWeapon()
{
	if (EquippedWeapon->IsEmpty())
	{
		//Reload weapon automatically, if firing button is still pressed
		ReloadWeapon();
	}

	if (!CheckCanFire())
		return;

	bIsCanFire = false;
	CrosshairShootingFactor = .75f;

	//Send fire event from client to server
	Server_FireWeapon(HitTarget);

	StartFireTimer();
}

bool UCombatComponent::CheckCanFire()
{
	if (!EquippedWeapon)
		return false;

	if (EquippedWeapon->IsEmpty())
		return false;

	if (!bIsCanFire)
		return false;

	if (CombatState != ECombatState::ECS_Unoccupied)
	{

		UE_LOG(LogTemp, Warning, TEXT("State is %d"), static_cast<UINT32>(CombatState));

		if (!(EquippedWeapon->GetIsReloadInterruptable() && CombatState == ECombatState::ECS_Reloading))
		{
			return false;
		}
	}

	return true;
}

bool UCombatComponent::CheckCanReload()
{
	if (!EquippedWeapon)
		return false;

	if (!CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
		return false;

	if (CarriedAmmoMap[EquippedWeapon->GetWeaponType()] == 0)
		return false;

	if (EquippedWeapon->IsFull())
		return false;

	return true;
}

void UCombatComponent::OnStateReload()
{
	if (!Character)
		return;

	Character->PlayReloadMontage();
}

void UCombatComponent::OnStateThrow()
{
	if (!Character)
		return;

	if (Character->IsLocallyControlled() && !Character->HasAuthority())
		return;

	Character->PlayThrowMontage();
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
	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
}

void UCombatComponent::EquipWeapon(class AWeaponBase* InWeaponToEquip)
{
	if (!GetIsUnoccupied())
		return;

	if (!Character)
		return;

	if (!InWeaponToEquip)
		return;

	if (EquippedWeapon)
	{
		DropWeapon();
	}

	EquippedWeapon = InWeaponToEquip;
	EquippedWeapon->ChangeWeaponState(EWeaponState::EWS_Equipped);

	const USkeletalMeshSocket* HandSocket{ Character->GetMesh()->GetSocketByName(FName("RightHandSocket")) };
	if (!HandSocket)
		return;

	HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	EquippedWeapon->SetOwner(Character);

	//Notify owner character to update "ammo" count if on server. If on client - it will happen in OnRep_Owner, on the line above
	if (Character->HasAuthority())
	{
		EquippedWeapon->NotifyOwner_Ammo();
	}

	PlayEquipSound();

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;

	UpdateCurrentCarriedAmmo(EquippedWeapon->GetWeaponType());
}

void UCombatComponent::SetIsAiming(bool bInIsAiming)
{
	bIsAiming = bInIsAiming;

	if (!EquippedWeapon)
		return;

	if (!Character || !Character->IsLocallyControlled())
		return;

	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SR)
	{
		Character->ShowSniperScopeWidget(bIsAiming);
	}
}

void UCombatComponent::SetIsFiring(bool bInIsFiring)
{
	//UE_LOG(LogTemp, Warning, TEXT("Component: Received firing"));

	bIsFiring = bInIsFiring;

	if (!bInIsFiring)
		return;

	if (!EquippedWeapon)
		return;

	FireWeapon();
}

void UCombatComponent::SetHitTarget(const FVector& TraceHitTarget)
{
	HitTarget = TraceHitTarget;
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (!EquippedWeapon)
		return;

	if (!Character)
		return;

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;

	//Equipped weapon is replicated, just as attaching an actor in Equip function.
	//There's no guarantee that EquippedWeapon will replicate earlier than attaching weapon to character.
	//So we'll also do these thing in this rep notify, just to be sure
	EquippedWeapon->ChangeWeaponState(EWeaponState::EWS_Equipped);

	const USkeletalMeshSocket* HandSocket{ Character->GetMesh()->GetSocketByName(FName("RightHandSocket")) };
	if (!HandSocket)
		return;

	HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	PlayEquipSound();
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	if (!EquippedWeapon)
		return;

	EquippedWeapon->NotifyOwner_Ammo();
}

void UCombatComponent::OnRep_CombatState()
{
	UE_LOG(LogTemp, Warning, TEXT("Repping combat state to %d, Is locally controlled: %d"), static_cast<uint32>(CombatState), Character->IsLocallyControlled());

	switch (CombatState)
	{
	case ECombatState::ECS_Unoccupied:
	{
		if (bIsFiring)
		{
			FireWeapon();
		}
	}
		break;
	case ECombatState::ECS_Reloading:
	{
		OnStateReload();
	}
		break;
	case ECombatState::ECS_Throwing:
	{
		OnStateThrow();
	}
	default:
		break;
	}
}

void UCombatComponent::OnShellInserted()
{
	if (!Character)
		return;

	if (Character->HasAuthority())
	{
		ReloadAmmo(1);
	}

	if (!CheckCanReload())
	{
		//Jump to ShotgunEndSection
		Character->PlayReloadEndMontage();
	}
}

void UCombatComponent::Server_FireWeapon_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	Multicast_FireWeapon(TraceHitTarget);
}

void UCombatComponent::Server_Throw_Implementation()
{
	CombatState = ECombatState::ECS_Throwing;

	OnStateThrow();
}

void UCombatComponent::Multicast_FireWeapon_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (!EquippedWeapon)
		return;
	 
	if (CombatState != ECombatState::ECS_Unoccupied)
	{
		if (!(EquippedWeapon->GetIsReloadInterruptable() && CombatState == ECombatState::ECS_Reloading))
			return;
	}

	EquippedWeapon->Fire(TraceHitTarget);
	CombatState = ECombatState::ECS_Unoccupied;

	if (!Character)
		return;

	Character->PlayFireMontage(bIsAiming);
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AR, StartingArAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SMG, StartingSmgAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, StartingShotgunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SR, StartingSniperAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_GranadeLauncher, StartingGranadeLauncherAmmo);
}

void UCombatComponent::UpdateCurrentCarriedAmmo(const EWeaponType WeaponType)
{
	if (!CarriedAmmoMap.Contains(WeaponType))
		return;

	CarriedAmmo = CarriedAmmoMap[WeaponType];
}

int32 UCombatComponent::CalculateAmountToReload(uint32 InRequestedAmount /*= 0*/)
{
	if (!EquippedWeapon)
		return 0;

	if (!CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
		return 0;

	int32 RoomInMag{ EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetWeaponAmmo() };
	int32 AmountCarried{ CarriedAmmoMap[EquippedWeapon->GetWeaponType()] };
	int32 Least{ FMath::Min(RoomInMag, AmountCarried) };
	
	uint32 ToReload{ static_cast<uint32>(FMath::Clamp(RoomInMag, 0, Least)) };
	if (InRequestedAmount > 0 && ToReload >= InRequestedAmount)
	{
		return InRequestedAmount;
	}

	return ToReload;
} 

void UCombatComponent::ReloadAmmo(uint32 InBulletsRequested /*= 0*/)
{
	if (!EquippedWeapon)
		return;

	if (!CheckCanReload())
		return;

	int32 AmountToReload{ CalculateAmountToReload(InBulletsRequested) };
	CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= AmountToReload;
	CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];

	EquippedWeapon->AddAmmo(-AmountToReload);
}

void UCombatComponent::PlayEquipSound()
{
	if (!Character)
		return;

	if (!EquippedWeapon)
		return;

	if (!EquippedWeapon->GetEquipSound())
		return;

	UGameplayStatics::PlaySoundAtLocation
	(
		this,
		EquippedWeapon->GetEquipSound(),
		Character->GetActorLocation()
	);
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	if (!GEngine)
		return;

	if (!GEngine->GameViewport)
		return;

	if (!Character)
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

	const double DistanceToCharacter{ (Character->GetActorLocation() - LinetraceStart).Size() };
	LinetraceStart += CrosshairsWorldDirection * (DistanceToCharacter + 50.0);

	//DrawDebugSphere(GetWorld(), LinetraceStart, 16.f, 12, FColor::Red, false);

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

void UCombatComponent::DropWeapon()
{
	if (!EquippedWeapon)
		return;

	EquippedWeapon->OnDropped();
	EquippedWeapon = nullptr;
}

void UCombatComponent::DropWeaponLaunch()
{
	if (!EquippedWeapon)
		return;

	EquippedWeapon->OnDropped();
	EquippedWeapon->GetWeaponMesh()->AddImpulse(Character->GetActorForwardVector() * 1000);
	EquippedWeapon = nullptr;
}

void UCombatComponent::ReloadWeapon()
{
	//Initialize reload process
	//e.g. start animation. Actual ammo reloading happens at the end of an animation
	//See ReloadAmmo()

	if (!Character)
		return;

	if (!GetIsUnoccupied())
		return;

	if (!CheckCanReload())
		return;

	CombatState = ECombatState::ECS_Reloading;
	if (Character->HasAuthority())
	{ 
		OnStateReload();
	}
}

void UCombatComponent::Throw()
{
	if (!GetIsUnoccupied())
		return;

	CombatState = ECombatState::ECS_Throwing;
	OnStateThrow();

	if (Character && !Character->HasAuthority())
	{
		Server_Throw();
	}
}

