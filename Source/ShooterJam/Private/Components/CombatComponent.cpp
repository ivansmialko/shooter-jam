// Made by smialko

#include "Components/CombatComponent.h"

#include "Weaponry/WeaponBase.h"
#include "Weaponry/Projectile.h"
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


void UCombatComponent::UpdateHudCrosshairs(float DeltaTime)
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
	bIsCanFire = true;

	if (!EquippedWeapon)
		return;

	if (!bIsFiring)
		return;

	if (!EquippedWeapon->GetIsAutomatic())
		return;

	//If firing button is still pressed, and weapon is automatic - shoot again automatically
	RequestFire();

	if (!EquippedWeapon->IsEmpty())
		return;

	//Reload weapon automatically, if firing button is still pressed
	ReloadWeapon();
}

void UCombatComponent::OnAnimReloadFinished()
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
			RequestFire();
		}
	}
}

void UCombatComponent::OnAnimThrowFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("Finished anim"));

	CombatState = ECombatState::ECS_Unoccupied;
	AttachActorToRightHand(EquippedWeapon);
}

void UCombatComponent::OnAnimThrowLaunched()
{
	RequestThrow();
}

void UCombatComponent::OnAnimSwapSwapped()
{
	if (!Character)
		return;

	if (!Character->HasAuthority())
		return;

	AWeaponBase* TempWeapon = EquippedWeapon;
	EquipPrimaryWeapon(SecondaryWeapon);
	EquipSecondaryWeapon(TempWeapon);
}

void UCombatComponent::OnAnimSwapFinished()
{
	if (!Character)
		return;

	if (!Character->HasAuthority())
		return;

	CombatState = ECombatState::ECS_Unoccupied;
}

void UCombatComponent::RequestFire()
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
	FVector Origin{ EquippedWeapon->GetMuzzleTransform().GetLocation() };
	TArray<FVector_NetQuantize> HitTargets{ EquippedWeapon->GetHitTargetsNet(Origin, HitTarget) };
	Server_FireWeapon(HitTargets);
	if (Character && !Character->HasAuthority())
	{
		ActionFire(HitTargets);
	}

	StartFireTimer();
}

void UCombatComponent::ActionFire(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	if (!EquippedWeapon)
		return;

	if (CombatState != ECombatState::ECS_Unoccupied)
	{
		if (!(EquippedWeapon->GetIsReloadInterruptable() && CombatState == ECombatState::ECS_Reloading))
			return;
	}

	EquippedWeapon->AddHitTarget(TraceHitTargets);
	EquippedWeapon->Fire();
	CombatState = ECombatState::ECS_Unoccupied;

	if (!Character)
		return;

	Character->PlayFireMontage(bIsAiming);
}

void UCombatComponent::RequestThrow()
{
	if (GrenadesAmount == 0)
		return;

	SetGrenadeVisibility(false);

	if (!Character)
		return;

	if (!Character->IsLocallyControlled())
		return;

	Server_ThrowGrenade(HitTarget);
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

	Character->PlayThrowMontage();
	AttachActorToLeftHand(EquippedWeapon);
	SetGrenadeVisibility(true);
}

void UCombatComponent::OnStateSwapping()
{
	if (!Character)
		return;

	//Play swap montage only on simulated proxies.
	//Server, as well as owning client playing animation by themselves
	if (Character->IsLocallyControlled())
		return;

	Character->PlaySwapMontage();
}

void UCombatComponent::OnStateDancing()
{
	if (!Character)
		return;

	Character->PlayDancingMontage();
}

void UCombatComponent::EquipPrimaryWeapon(AWeaponBase* InWeaponToEquip, bool bInDropPrevious /*= false*/)
{

#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("[%s] Combat component equips weapon %s to primary"), (Character->GetLocalRole() == ENetRole::ROLE_Authority ? TEXT("Server") : TEXT("Client")), (*InWeaponToEquip->GetActorLabel()));
#endif
	if (bInDropPrevious)
	{
		DropWeapon();
	}

	EquippedWeapon = InWeaponToEquip;
	UpdateCurrentCarriedAmmo(EquippedWeapon->GetWeaponType());
	EquippedWeapon->ChangeWeaponState(EWeaponState::EWS_Equipped);
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->NotifyOwner_Ammo();

	AttachActorToRightHand(EquippedWeapon);
	PlayEquipSound(EquippedWeapon);
}

void UCombatComponent::EquipSecondaryWeapon(AWeaponBase* InWeaponToEquip)
{
	SecondaryWeapon = InWeaponToEquip;

	if (!SecondaryWeapon)
		return;

#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("[%s] Combat component equips weapon %s to secondary"), (Character->GetLocalRole() == ENetRole::ROLE_Authority ? TEXT("Server") : TEXT("Client")), (*InWeaponToEquip->GetActorLabel()));
#endif
	SecondaryWeapon->ChangeWeaponState(EWeaponState::EWS_EquippedSecondary);
	SecondaryWeapon->SetOwner(Character);

	AttachActorToBackpack(InWeaponToEquip);
	PlayEquipSound(SecondaryWeapon);
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (Character && Character->IsLocallyControlled())
	{
		UpdateHudCrosshairs(DeltaTime);

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
	DOREPLIFETIME(UCombatComponent, SecondaryWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME(UCombatComponent, GrenadesAmount);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
}

void UCombatComponent::EquipWeapon(class AWeaponBase* InWeaponToEquip)
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("[%s] Combat component equips weapon %s"), (Character->GetLocalRole() == ENetRole::ROLE_Authority ? TEXT("Server") : TEXT("Client")), (*InWeaponToEquip->GetActorLabel()));
#endif

	if (!GetIsUnoccupied())
		return;

	if (!Character)
		return;

	if (!InWeaponToEquip)
		return;

	if (EquippedWeapon && !SecondaryWeapon)
	{
		EquipSecondaryWeapon(InWeaponToEquip);
	}
	else
	{
		EquipPrimaryWeapon(InWeaponToEquip, true);
	}

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::SwapWeapons()
{
	if (CombatState != ECombatState::ECS_Unoccupied)
		return;

	if (!Character)
		return;

	Character->PlaySwapMontage();
	CombatState = ECombatState::ECS_Swapping;
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

	bIsAimingPressed = bInIsAiming;
}

void UCombatComponent::SetIsFiring(bool bInIsFiring)
{
	//UE_LOG(LogTemp, Warning, TEXT("Component: Received firing"));

	bIsFiring = bInIsFiring;

	if (!bInIsFiring)
		return;

	if (!EquippedWeapon)
		return;

	RequestFire();
}

void UCombatComponent::SetHitTarget(const FVector& TraceHitTarget)
{
	HitTarget = TraceHitTarget;
}

void UCombatComponent::SetWeaponsUseSsr(bool bInUseSsr)
{
	if (EquippedWeapon && EquippedWeapon->GetIsUsingSsr())
	{
		EquippedWeapon->SetCurrentlyUseSsr(bInUseSsr);
	}
	if (SecondaryWeapon && SecondaryWeapon->GetIsUsingSsr())
	{
		SecondaryWeapon->SetCurrentlyUseSsr(bInUseSsr);
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (!Character)
		return;

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;

	if (!EquippedWeapon)
		return;

	//Equipped weapon is replicated, just as attaching an actor in Equip function.
	//There's no guarantee that EquippedWeapon will replicate earlier than attaching weapon to character.
	//So we'll also do these thing in this rep notify, just to be sure
	EquippedWeapon->ChangeWeaponState(EWeaponState::EWS_Equipped);

	AttachActorToRightHand(EquippedWeapon);
	PlayEquipSound(EquippedWeapon);
}

void UCombatComponent::OnRep_SecondaryWeapon()
{
	if (!Character)
		return;

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;

	if (!SecondaryWeapon)
		return;

	//Secondary weapon is replicated, just as attaching an actor in Equip function.
	//There's no guarantee that SecondaryWeapon will replicate earlier than attaching weapon to character.
	//So we'll also do these thing in this rep notify, just to be sure
	SecondaryWeapon->ChangeWeaponState(EWeaponState::EWS_Equipped);

	AttachActorToBackpack(SecondaryWeapon);
	PlayEquipSound(SecondaryWeapon);
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	if (!EquippedWeapon)
		return;

	EquippedWeapon->NotifyOwner_Ammo();
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Unoccupied:
	{
		if (bIsFiring)
		{
			RequestFire();
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
	case ECombatState::ECS_Swapping:
	{
		OnStateSwapping();
	}
	case ECombatState::ECS_Dancing:
	{
		OnStateDancing();
	}
	default:
		break;
	}
}

void UCombatComponent::OnRep_GrenadesAmount()
{
	UpdateHudGrenades();
}

void UCombatComponent::OnRep_IsAiming()
{
	if (!Character || !Character->IsLocallyControlled())
		return;

	bIsAiming = bIsAimingPressed;
}

void UCombatComponent::OnAnimShellInserted()
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

void UCombatComponent::Server_FireWeapon_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	Multicast_FireWeapon(TraceHitTargets);
}

void UCombatComponent::Server_Throw_Implementation()
{
	if (GrenadesAmount == 0)
		return;

	CombatState = ECombatState::ECS_Throwing;
	OnStateThrow();

	GrenadesAmount = FMath::Clamp(GrenadesAmount - 1, 0, GrenadesAmountMax);
	UpdateHudGrenades();
}

void UCombatComponent::Server_ThrowGrenade_Implementation(const FVector_NetQuantize& Target)
{
	if (!GrenadeProjectileClass)
		return;

	if (!Character)
		return;

	if (!Character->HasAuthority())
		return;

	if (!Character->GetGrenadeMesh())
		return;

	const FVector StartingLocation = Character->GetGrenadeMesh()->GetComponentLocation();
	FVector Direction = Target - StartingLocation;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Character;
	SpawnParams.Instigator = Character;

	UWorld* World = GetWorld();
	if (!World)
		return;

	World->SpawnActor<AProjectile>(GrenadeProjectileClass, StartingLocation, Direction.Rotation(), SpawnParams);
}

void UCombatComponent::Server_StartDancing_Implementation(FName EmotionName)
{
	CombatState = ECombatState::ECS_Dancing;
	OnStateDancing();
}

void UCombatComponent::Server_StopDancing_Implementation()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (!Character)
		return;

	Character->StopDancingMontage();
}

void UCombatComponent::Multicast_FireWeapon_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority())
		return;

	ActionFire(TraceHitTargets);
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

void UCombatComponent::FireProjectile()
{

}

void UCombatComponent::FireHitScan()
{

}

void UCombatComponent::FireMultiHitScan()
{

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

	EquippedWeapon->AddAmmo(AmountToReload);
}

void UCombatComponent::AttachActorToRightHand(AActor* InActor)
{
	if (!InActor)
		return;

	if (!Character)
		return;

	if (!Character->GetMesh())
		return;

	const USkeletalMeshSocket* HandSocket{ Character->GetMesh()->GetSocketByName(FName("RightHandSocket")) };
	if (!HandSocket)
		return;

	HandSocket->AttachActor(InActor, Character->GetMesh());
}

void UCombatComponent::AttachActorToLeftHand(AActor* InActor)
{
	if (!InActor)
		return;

	if (!Character)
		return;

	if (!Character->GetMesh())
		return;

	if (!EquippedWeapon)
		return;

	FName SocketName("LeftHandSocket");
	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol ||
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SMG)
	{
		SocketName = FName("LeftHandPistolSocket");
	}

	const USkeletalMeshSocket* HandSocket{ Character->GetMesh()->GetSocketByName(SocketName) };
	if (!HandSocket)
		return;

	HandSocket->AttachActor(InActor, Character->GetMesh());
}

void UCombatComponent::AttachActorToBackpack(AActor* InActor)
{
	if (!InActor)
		return;

	if (!Character)
		return;

	if (!Character->GetMesh())
		return;

	const USkeletalMeshSocket* BackpackSocket{ Character->GetMesh()->GetSocketByName(FName("BackpackSocket")) };
	if (!BackpackSocket)
		return;

#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("[%s][%s] Combat component attaching %s to backpack"),
		(Character->GetLocalRole() == ENetRole::ROLE_Authority ? TEXT("Server") : TEXT("Client")),
		(Character->IsLocallyControlled() ? TEXT("Local") : TEXT("Remote")),
		(*InActor->GetActorLabel()));
#endif

	BackpackSocket->AttachActor(InActor, Character->GetMesh());
}

void UCombatComponent::SetGrenadeVisibility(bool bVisible)
{
	if (!Character)
		return;

	Character->SetGrenadeVisibility(bVisible);
}

void UCombatComponent::PlayEquipSound(AWeaponBase* WeaponToEquip)
{
	if (!Character)
		return;

	UGameplayStatics::PlaySoundAtLocation
	(
		this,
		WeaponToEquip->GetEquipSound(),
		Character->GetActorLocation()
	);
}

void UCombatComponent::UpdateHudGrenades()
{
	if (!Character)
		return;

	Character->HudUpdateGrenades();
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
	LinetraceStart += CrosshairsWorldDirection * (DistanceToCharacter + HitTargetOffset);

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

bool UCombatComponent::DropWeaponLaunch()
{
	if (!EquippedWeapon)
		return false;

	//Do not drop equipped weapon, if there are no secondary weapon
	if (!SecondaryWeapon)
		return false;

	//Drop weapon
	EquippedWeapon->OnDropped();
	EquippedWeapon->GetWeaponMesh()->AddImpulse(Character->GetActorForwardVector() * WeaponDropImpulse);
	EquippedWeapon = nullptr;

	SwapWeapons();

	return true;
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

	if (!EquippedWeapon)
		return;

	CombatState = ECombatState::ECS_Throwing;
	OnStateThrow();

	if (Character && !Character->HasAuthority())
	{
		Server_Throw();
	}

	if (Character && Character->HasAuthority())
	{
		GrenadesAmount = FMath::Clamp(GrenadesAmount - 1, 0, GrenadesAmountMax);
		UpdateHudGrenades();
	}
}

void UCombatComponent::PickupAmmo(EWeaponType InWeaponType, int32 InAmmoAmount)
{
	if (!CarriedAmmoMap.Contains(InWeaponType))
		return;

	CarriedAmmoMap[InWeaponType] += InAmmoAmount;

	if (!EquippedWeapon)
		return;

	UpdateCurrentCarriedAmmo(EquippedWeapon->GetWeaponType());

	if (EquippedWeapon->IsEmpty() && EquippedWeapon->GetWeaponType() == InWeaponType)
	{
		ReloadWeapon();
	}
}

void UCombatComponent::ChangeCombatState(ECombatState InCombatState)
{
	CombatState = InCombatState;
}

void UCombatComponent::StartDancing()
{
	if (!Character)
		return;

	if (!GetIsUnoccupied())
		return;

	CombatState = ECombatState::ECS_Dancing;
	OnStateDancing();
}

void UCombatComponent::StopDancing()
{
	if (!Character)
		return;

	if (!GetIsDancing())
		return;

	CombatState = ECombatState::ECS_Unoccupied;
	Character->StopDancingMontage();
}
