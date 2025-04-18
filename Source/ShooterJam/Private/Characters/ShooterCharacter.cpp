// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ShooterCharacter.h"

#include "Weaponry/WeaponBase.h"
#include "Animations/ShooterCharacterAnimInstance.h"
#include "Components/CombatComponent.h"
#include "Components/BuffComponent.h"
#include "Game/ShooterJam.h"
#include "GameModes/ShooterGameMode.h"
#include "PlayerControllers/ShooterCharacterController.h"
#include "PlayerState/ShooterPlayerState.h"
#include "Weaponry/WeaponTypes.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"

AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	UCharacterMovementComponent* MovementComponent { GetCharacterMovement() };
	MovementComponent->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);

	BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	BuffComponent->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 720.f);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	GrenadeMesh->SetupAttachment(GetMesh(), FName("RightHandGrenadeSocket"));
	GrenadeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AShooterCharacter::OnReceiveDamage);
	}

	SetGrenadeVisibility(false);
}

void AShooterCharacter::Restart()
{
	Super::Restart();

	//Using this, because at BeginPlay stage we have no player controller
	//And because ShooterCharacterController::OnPossess not called on client
	InitInputs();
}

void AShooterCharacter::OnMove(const FInputActionValue& Value)
{
	if (!GetController())
		return;

	if (!bGameplayEnabled)
		return;

	FVector2D InputVector{ Value.Get<FVector2D>() };
	const FRotator Rotation{ GetController()->GetControlRotation() };
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection{ FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) };
	const FVector RightDirection{ FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) };

	AddMovementInput(ForwardDirection, InputVector.Y);
	AddMovementInput(RightDirection, InputVector.X);
}

void AShooterCharacter::OnLook(const FInputActionValue& Value)
{
	if(!GetController())
		return;

	FVector2D LookVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void AShooterCharacter::OnJump(const FInputActionValue& Value)
{
	if (!bGameplayEnabled)
		return;

	Jump();
}

void AShooterCharacter::OnEquip(const FInputActionValue& Value)
{
	if (!bGameplayEnabled)
		return;

	if (HasAuthority())
	{
		ActionEquip();
	}
	else
	{
		Server_OnEquip();
	}
}


void AShooterCharacter::OnCrouch(const FInputActionValue& Value)
{
	if (!bGameplayEnabled)
		return;

	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AShooterCharacter::OnAimStart(const FInputActionValue& Value)
{
	if (!bGameplayEnabled)
		return;

	ActionAimStart();

	if (!HasAuthority())
	{
		Server_OnAimStart();
	}
}

void AShooterCharacter::OnAimEnd(const FInputActionValue& Value)
{
	if (!bGameplayEnabled)
		return;

	ActionAimEnd();

	if (!HasAuthority())
	{
		Server_OnAimEnd();
	}
}

void AShooterCharacter::OnFireStart(const FInputActionValue& Value)
{
	if (!bGameplayEnabled)
		return;

	if (!CombatComponent)
		return;

	CombatComponent->SetIsFiring(true);
}

void AShooterCharacter::OnFireEnd(const FInputActionValue& Value)
{
	if (!bGameplayEnabled)
		return;

	if (!CombatComponent)
		return;

	CombatComponent->SetIsFiring(false);
}

void AShooterCharacter::OnDropWeapon(const FInputActionValue& Value)
{
	if (!CombatComponent)
		return;

	CombatComponent->DropWeaponLaunch();

	if (!PlayerController)
		return;

	if (!PlayerController->GetPlayerHud())
		return;

	PlayerController->GetPlayerHud()->SetWeaponAmmoEmpty();
	PlayerController->GetPlayerHud()->SetCarriedAmmoEmpty();
}

void AShooterCharacter::OnReload(const FInputActionValue& Value)
{
	if (!bGameplayEnabled)
		return;

	if (HasAuthority())
	{
		ActionReload();
	}
	else
	{
		Server_OnReload();
	}
}

void AShooterCharacter::OnThrow(const FInputActionValue& Value)
{
	ActionThrow();
}

//Received only on the server. Clients receive damage as replication of Health variable. See OnRep_Health
void AShooterCharacter::OnReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageTypem, class AController* InstigatorController, AActor* DamageCauser)
{
	if (bIsEliminated)
		return;

	float DamageToHealth = Damage;
	if (Shield > 0.f)
	{
		if (Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - Damage, 0, MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0, Damage);
			Shield = 0.f;
		}
	}

	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);

	//It's for server to play player-server effects
	if (HasAuthority())
	{
		ActionReceiveDamage();
		HudUpdateHealth();
		HudUpdateShield();
	}

	if (Health > 0.f)
		return;

	AShooterGameMode* GameMode = GetShooterGameMode();
	if (!GameMode)
		return;

	AShooterCharacterController* AttackerController = Cast<AShooterCharacterController>(InstigatorController);
	if (!AttackerController)
		return;

	GameMode->OnPlayerEliminated(this, PlayerController, AttackerController);
}

void AShooterCharacter::OnEliminatedTimerFinished()
{
	AShooterGameMode* GameMode = GetShooterGameMode();
	if (!GameMode)
		return;

	GameMode->RequestRespawn(this, GetController());
}

void AShooterCharacter::CalculateAimOffset(float DeltaTime)
{
	if (!CombatComponent)
		return;

	if (!CombatComponent->GetIsWeaponEquipped())
		return;

	float Speed{ CalculateSpeed() };
	bool bIsInAir{ GetCharacterMovement()->IsFalling() };

	if (Speed == 0.f && !bIsInAir) //Standing still and not jumping
	{
		FRotator CurrentAimRotation{ FRotator(0.f, GetBaseAimRotation().Yaw, 0.f) };
		FRotator DeltaAimRotation{ UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation) };
		AO_Yaw = DeltaAimRotation.Yaw;
		bUseControllerRotationYaw = true;
		bRotateRootBone = true;

		CalculateTurningInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) //Running or jumping
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		bRotateRootBone = false;
 
		TurningInPlace = ETurningInPlace::TIP_NotTurning;
	}
}

void AShooterCharacter::CalculateAimPitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void AShooterCharacter::CalculateTurningInPlace(float DeltaTime)
{
	if (TurningInPlace == ETurningInPlace::TIP_NotTurning)
	{
		Root_AO_Yaw = AO_Yaw;
	}

	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::TIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::TIP_Left;
	}

	if (TurningInPlace != ETurningInPlace::TIP_NotTurning)
	{
		Root_AO_Yaw = FMath::FInterpTo(Root_AO_Yaw, 0, DeltaTime, 4.f);
		AO_Yaw = Root_AO_Yaw;

		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::TIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

float AShooterCharacter::CalculateSpeed() const
{
	FVector CharacterVelocity{ GetVelocity() };
	CharacterVelocity.Z = 0.f;
	return static_cast<float>(CharacterVelocity.Size());
}

void AShooterCharacter::PollInitPlayerState()
{
	if (PlayerState)
		return;

	PlayerState = GetPlayerState<AShooterPlayerState>();
	if (!PlayerState)
		return;

	PlayerState->UpdateScoreHud();
	PlayerState->UpdateDefeatsHud();
}

void AShooterCharacter::PollInitPlayerController()
{
	if (PlayerController)
		return;

	PlayerController = GetController<AShooterCharacterController>();
}

void AShooterCharacter::PollInitPlayerHud()
{
	if (bHudInitialized)
		return;

	if (!PlayerController)
		return;

	if (!PlayerController->GetPlayerHud())
		return;

	if (!PlayerController->GetPlayerHud()->IsOverlayInitialized())
		return;

	bHudInitialized = true;

	HudUpdate();
}

void AShooterCharacter::InitInputs()
{
	if (bInputInitialized)
		return;

	APlayerController* ShooterPlayerController = Cast<APlayerController>(GetController());
	if (!ShooterPlayerController)
		return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ShooterPlayerController->GetLocalPlayer());
	if (!Subsystem)
		return;

	//FString LocalRoleString = UEnum::GetDisplayValueAsText<ENetRole>(GetLocalRole()).ToString();
	//FString SysMessage = FString::Printf(TEXT("Player %s, %d, is local: %s."), *LocalRoleString, static_cast<int>(GetNetMode()), (IsLocallyControlled() ? TEXT("true") : TEXT("false")));
	//FString UserMessage = FString::Printf(TEXT("Adding mapping context, i'm %s."), *GetName());
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("%s%s"), *SysMessage, *UserMessage));

	Subsystem->AddMappingContext(InputMappingContext, 0);
	bInputInitialized = true;
}

void AShooterCharacter::CalculateAimOffset_SimProxies()
{
	if (!CombatComponent)
		return;

	if (!CombatComponent->GetEquippedWeapon())
		return;
	
	bRotateRootBone = false;

	float Speed{ CalculateSpeed() };
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::TIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > ProxyTurnTreshold)
	{
		if (ProxyYaw > ProxyTurnTreshold)
		{
			TurningInPlace = ETurningInPlace::TIP_Right;
		}
		else if (ProxyYaw < -ProxyTurnTreshold)
		{
			TurningInPlace = ETurningInPlace::TIP_Right;
		}
		else
		{
			TurningInPlace = ETurningInPlace::TIP_NotTurning;
		}

		return;
	}

	TurningInPlace = ETurningInPlace::TIP_NotTurning; 
}

void AShooterCharacter::CheckHidePlayerIfCameraClose()
{
	if (!IsLocallyControlled())
		return;

	if (!FollowCamera)
		return;

	if (!GetMesh())
		return;

	double DistanceFromCameraToCharacter{ (FollowCamera->GetComponentLocation() - GetActorLocation()).Size() };

	GetMesh()->SetVisibility(DistanceFromCameraToCharacter > CameraDistanceTreshold);

	if (!CombatComponent || !CombatComponent->GetEquippedWeapon())
		return;

	CombatComponent->GetEquippedWeapon()->GetWeaponMesh()->bOwnerNoSee = (DistanceFromCameraToCharacter < CameraDistanceTreshold);
}

void AShooterCharacter::OnRep_OverlappingWeapon(AWeaponBase* LastOverlappedWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(true);
	}

	if (LastOverlappedWeapon && !OverlappingWeapon)
	{
		LastOverlappedWeapon->ShowPickUpWidget(false);
	}
}

void AShooterCharacter::OnRep_Health(float LastHealth)
{
	if (Health < LastHealth)
	{
		ActionReceiveDamage();
	}
	HudUpdateHealth();
}

void AShooterCharacter::OnRep_Shield(float LastShield)
{
	if (Shield < LastShield)
	{
		ActionReceiveDamage();
	}

	HudUpdateShield();
}

void AShooterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	CalculateAimOffset_SimProxies();

	TimeSinceLastMovementRep = 0.f;
}

void AShooterCharacter::Server_OnEquip_Implementation()
{
	ActionEquip();
}

void AShooterCharacter::Server_OnAimStart_Implementation()
{
	ActionAimStart();
}

void AShooterCharacter::Server_OnAimEnd_Implementation()
{
	ActionAimEnd();
}

void AShooterCharacter::Server_OnReload_Implementation()
{
	ActionReload();
}

void AShooterCharacter::Multicast_OnEliminated_Implementation()
{
	bIsEliminated = true;
	PlayEliminationMontage();

	PlayDissolvingEffect();
	PlayElimbotEffect();

	DisableInputs();

	if (!PlayerController)
		return;

	if (!PlayerController->GetPlayerHud())
		return;

	PlayerController->GetPlayerHud()->SetWeaponAmmo(0);
}

void AShooterCharacter::ActionEquip()
{
	if (!CombatComponent)
		return;

	CombatComponent->EquipWeapon(OverlappingWeapon);
}

void AShooterCharacter::ActionAimStart()
{
	if (!CombatComponent)
		return;

	CombatComponent->SetIsAiming(true);
	GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;
}

void AShooterCharacter::ActionAimEnd()
{
	if (!CombatComponent)
		return;

	CombatComponent->SetIsAiming(false);
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}

void AShooterCharacter::ActionReceiveDamage()
{
	PlayHitReactMontage();
}

void AShooterCharacter::ActionReload()
{
	if (!CombatComponent)
		return;

	CombatComponent->ReloadWeapon();
}

void AShooterCharacter::ActionThrow()
{
	if (!CombatComponent)
		return;

	CombatComponent->Throw();
}

void AShooterCharacter::DisableInputs()
{
	//Disable movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	if (PlayerController)
	{
		DisableInput(PlayerController);
	}

	//Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AShooterCharacter::DropWeapon()
{
	if (!CombatComponent)
		return;

	if (!CombatComponent->GetEquippedWeapon())
		return;

	CombatComponent->GetEquippedWeapon()->OnDropped();
}

void AShooterCharacter::SetOverlappingWeapon(AWeaponBase* Weapon)
{
	//Save the last overlapped weapon, to call local function with this value
	AWeaponBase* LastWeaponOverlapped{ OverlappingWeapon };

	//Replicates to client
	OverlappingWeapon = Weapon;

	//Call method manually, because variable does not replicate to server
	//And this code will ONLY run on the server, becase sphere overlapping works only on the server, as mentioned in AWeaponBase::BeginPlay
	if (IsLocallyControlled())
	{
		OnRep_OverlappingWeapon(LastWeaponOverlapped);
	}
}

void AShooterCharacter::SetGrenadeVisibility(bool bVisible)
{
	if (!GrenadeMesh)
		return;

	GrenadeMesh->SetVisibility(bVisible);
}

void AShooterCharacter::SetWalkSpeed(const float InWalkSpeed)
{
	if (!GetCharacterMovement())
		return;

	GetCharacterMovement()->MaxWalkSpeed = InWalkSpeed;
}

void AShooterCharacter::SetCrouchSpeed(const float InCrouchSpeed)
{
	if (!GetCharacterMovement())
		return;

	GetCharacterMovement()->MaxWalkSpeedCrouched = InCrouchSpeed;
}

void AShooterCharacter::SetJumpVelocity(const float InJumpVelocity)
{
	if (!GetCharacterMovement())
		return;

	GetCharacterMovement()->JumpZVelocity = InJumpVelocity;
}

void AShooterCharacter::DisableGameplay()
{
	bGameplayEnabled = false;
	CombatComponent->SetIsAiming(false);
	CombatComponent->SetIsFiring(false);
	bUseControllerRotationYaw = false;
	TurningInPlace = ETurningInPlace::TIP_NotTurning;
}

void AShooterCharacter::HudUpdate()
{
	ENetRole LocalNetRole = GetLocalRole();
	if (LocalNetRole == ROLE_AutonomousProxy)
	{
		int A = 10;
		A = 10;
	}

	HudUpdateAmmo();
	HudUpdateHealth();
	HudUpdateShield();
	HudUpdateGrenades();
}

AWeaponBase* AShooterCharacter::GetEquippedWeapon() const
{
	if (!CombatComponent)
		return nullptr;

	return CombatComponent->GetEquippedWeapon();
}

UStaticMeshComponent* AShooterCharacter::GetGrenadeMesh() const
{
	return GrenadeMesh;
}

FVector AShooterCharacter::GetHitTarget() const
{
	FVector HitTarget;
	if (CombatComponent)
	{
		HitTarget = CombatComponent->GetHitTarget();
	}

	return HitTarget;
}

void AShooterCharacter::PlayFireMontage(bool bInIsAiming)
{
	if (!CombatComponent)
		return;

	if (!CombatComponent->GetEquippedWeapon())
		return;

	if (!GetMesh())
		return;

	if (!FireWeaponMontage)
		return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
		return;

	AnimInstance->Montage_Play(FireWeaponMontage);
	
	FName SectionName{ (bInIsAiming ? FName("RifleAim") : FName("RifleHip")) };
	AnimInstance->Montage_JumpToSection(SectionName);
}

void AShooterCharacter::PlayEliminationMontage()
{
	if (!GetMesh())
		return;

	if (!EliminationMontage)
		return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
		return;

	AnimInstance->Montage_Play(EliminationMontage);
}

void AShooterCharacter::PlayReloadMontage(bool bInPlayReloadEnd)
{
	if (!CombatComponent)
		return;

	if (!CombatComponent->GetEquippedWeapon())
		return;

	if (!GetMesh())
		return;

	if (!ReloadMontage)
		return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
		return;

	FString SectionName;
	switch (CombatComponent->GetEquippedWeapon()->GetWeaponType())
	{
	case EWeaponType::EWT_AR:
		SectionName = TEXT("Rifle");
		break;
	case EWeaponType::EWT_RocketLauncher:
		SectionName = TEXT("RocketLauncher");
		break;
	case EWeaponType::EWT_Pistol:
		SectionName = TEXT("Pistol");
		break;
	case EWeaponType::EWT_SMG:
		SectionName = TEXT("Pistol");
		break;
	case EWeaponType::EWT_Shotgun:
		SectionName = TEXT("Shotgun");
		break;
	case EWeaponType::EWT_SR:
		SectionName = TEXT("SR");
		break;
	case EWeaponType::EWT_GranadeLauncher:
		SectionName = TEXT("Rifle");
		break;
	default:
		break;
	}

	if (bInPlayReloadEnd)
	{
		SectionName += "End";
	}

	AnimInstance->Montage_Play(ReloadMontage);
	AnimInstance->Montage_JumpToSection(FName(SectionName));
}

void AShooterCharacter::PlayReloadMontage()
{
	PlayReloadMontage(false);
}

void AShooterCharacter::PostInitializeCombatComponent()
{
	if (!CombatComponent)
		return;

	CombatComponent->Character = this;
	CombatComponent->PrimaryComponentTick.bCanEverTick = true;
}

void AShooterCharacter::PostInitializeBuffComponent()
{
	if (!BuffComponent)
		return;

	BuffComponent->SetCharacter(this); 

	if (auto Movement = GetCharacterMovement())
	{
		BuffComponent->SetInitialBaseSpeed(Movement->MaxWalkSpeed);
		BuffComponent->SetInitialBaseSpeed(Movement->MaxWalkSpeedCrouched);
	}
}

void AShooterCharacter::PlayReloadEndMontage()
{
	PlayReloadMontage(true);
}

void AShooterCharacter::PlayThrowMontage()
{
	if (!GetMesh())
		return;

	if (!ThrowMontage)
		return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
		return;

	AnimInstance->Montage_Play(ThrowMontage);
	//AnimInstance->Montage_JumpToSection(FName(SectionName));
}

void AShooterCharacter::OnReloadFinished()
{
	if (!CombatComponent)
		return;

	CombatComponent->OnReloadFinished();
}

void AShooterCharacter::OnShellInserted()
{
	if (!CombatComponent)
		return;

	CombatComponent->OnShellInserted();
}

void AShooterCharacter::OnThrowFinished()
{
	if (!CombatComponent)
		return;

	CombatComponent->OnThrowFinished();
}

void AShooterCharacter::OnThrowLaunched()
{
	if (!CombatComponent)
		return;

	CombatComponent->OnThrowLaunched();
}

void AShooterCharacter::PlayHitReactMontage()
{
	if (!CombatComponent)
		return;

	if (!CombatComponent->GetEquippedWeapon())
		return;

	if (!GetMesh())
		return;

	if (!FireWeaponMontage)
		return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
		return;

	AnimInstance->Montage_Play(HitReactMontage);

	FName SectionName{ "FromFront" };
	AnimInstance->Montage_JumpToSection(SectionName);
}

void AShooterCharacter::PlayDissolvingEffect()
{
	if (!DissolveCurve)
		return;

	if (!DissolveTimeline)
		return;

	DissolveMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);

	if (!DissolveMaterialInstanceDynamic)
		return;

	DissolveMaterialInstanceDynamic->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
	DissolveMaterialInstanceDynamic->SetScalarParameterValue(TEXT("Glow"), 200.f);
	GetMesh()->SetMaterial(0, DissolveMaterialInstanceDynamic);

	DissolveTrackDlg.BindDynamic(this, &AShooterCharacter::TimelineUpdateDissolveMaterial);

	DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrackDlg);
	DissolveTimeline->Play();
}

void AShooterCharacter::PlayElimbotEffect()
{
	if (!ElimbotEffect)
		return;

	FVector ElimbotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
	ElimbotComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ElimbotEffect, FTransform(GetActorRotation(), ElimbotSpawnPoint));

	if (!ElimbotSound)
		return;

	UGameplayStatics::SpawnSoundAtLocation(this, ElimbotSound, GetActorLocation());
}

void AShooterCharacter::HudUpdateHealth()
{
	if (!PlayerController)
	{
		PlayerController = Cast<AShooterCharacterController>(GetController());
	}

	if (!PlayerController)
		return;

	if (!PlayerController->GetPlayerHud())
		return;

	PlayerController->GetPlayerHud()->SetHealth(Health, MaxHealth);
}

void AShooterCharacter::HudUpdateShield()
{
	if (!PlayerController)
	{
		PlayerController = Cast<AShooterCharacterController>(GetController());
	}

	if (!PlayerController)
		return;

	if (!PlayerController->GetPlayerHud())
		return;

	PlayerController->GetPlayerHud()->SetShield(Shield, MaxShield);
}

void AShooterCharacter::HudUpdateAmmo()
{
	if (!PlayerController)
	{
		PlayerController = Cast<AShooterCharacterController>(GetController());
	}

	if (!PlayerController)
		return;

	if (!PlayerController->GetPlayerHud())
		return;

	if (!CombatComponent)
		return;

	if (!CombatComponent->GetEquippedWeapon())
	{
		PlayerController->GetPlayerHud()->SetWeaponAmmoEmpty();
		PlayerController->GetPlayerHud()->SetCarriedAmmoEmpty();
		return;
	}

	PlayerController->GetPlayerHud()->SetWeaponAmmo(CombatComponent->GetWeaponAmmo());
	PlayerController->GetPlayerHud()->SetCarriedAmmo(CombatComponent->GetCarriedAmmo());
}

void AShooterCharacter::HudUpdateGrenades()
{
	if (!PlayerController)
	{
		PlayerController = Cast<AShooterCharacterController>(GetController());
	}

	if (!PlayerController)
		return;

	if (!PlayerController->GetPlayerHud())
		return;

	if (!CombatComponent)
		return;

	PlayerController->GetPlayerHud()->SetGrenadesAmount(CombatComponent->GetGrenadesAmount());
}

void AShooterCharacter::TimelineUpdateDissolveMaterial(float InDissolveValue)
{
	if (!DissolveMaterialInstanceDynamic)
		return;

	DissolveMaterialInstanceDynamic->SetScalarParameterValue(TEXT("Dissolve"), InDissolveValue);
}

AShooterGameMode* AShooterCharacter::GetShooterGameMode() const
{
	return GetWorld()->GetAuthGameMode<AShooterGameMode>();
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy
		&& IsLocallyControlled()
		&& bGameplayEnabled)
	{
		CalculateAimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementRep += DeltaTime;
		if (TimeSinceLastMovementRep > 0.25)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAimPitch();
	}

	//CheckHidePlayerIfCameraClose();
	PollInitPlayerState();
	PollInitPlayerController();
	PollInitPlayerHud();
}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if(!EnhancedInput)
		return;

	EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterCharacter::OnMove);
	EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::OnLook);
	EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AShooterCharacter::OnJump);
	EnhancedInput->BindAction(EquipAction, ETriggerEvent::Started, this, &AShooterCharacter::OnEquip);
	EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Started, this, &AShooterCharacter::OnCrouch);
	EnhancedInput->BindAction(AimAction, ETriggerEvent::Started, this, &AShooterCharacter::OnAimStart);
	EnhancedInput->BindAction(AimAction, ETriggerEvent::Completed, this, &AShooterCharacter::OnAimEnd);
	EnhancedInput->BindAction(FireAction, ETriggerEvent::Started, this, &AShooterCharacter::OnFireStart);
	EnhancedInput->BindAction(FireAction, ETriggerEvent::Completed, this, &AShooterCharacter::OnFireEnd);
	EnhancedInput->BindAction(DropWeaponAction, ETriggerEvent::Triggered, this, &AShooterCharacter::OnDropWeapon);
	EnhancedInput->BindAction(ReloadAction, ETriggerEvent::Started, this, &AShooterCharacter::OnReload);
	EnhancedInput->BindAction(ThrowAction, ETriggerEvent::Started, this, &AShooterCharacter::OnThrow);
}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AShooterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AShooterCharacter, Health);
	DOREPLIFETIME(AShooterCharacter, Shield);
	DOREPLIFETIME(AShooterCharacter, bGameplayEnabled);
}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	PostInitializeCombatComponent();
	PostInitializeBuffComponent();
}

void AShooterCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void AShooterCharacter::OnEliminated()
{
	Multicast_OnEliminated();
	DropWeapon();

	GetWorldTimerManager().SetTimer(EliminatedTimer, this, &AShooterCharacter::OnEliminatedTimerFinished, EliminationDelay);
}

void AShooterCharacter::OnSpendRound(AWeaponBase* InWeapon)
{
	HudUpdateAmmo();
}

void AShooterCharacter::Destroyed()
{
	Super::Destroy();

	if (!ElimbotComponent)
		return;

	ElimbotComponent->DestroyComponent();
}

