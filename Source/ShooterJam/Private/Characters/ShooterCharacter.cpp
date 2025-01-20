// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ShooterCharacter.h"

#include "Weaponry/WeaponBase.h"
#include "Animations/ShooterCharacterAnimInstance.h"
#include "Components/CombatComponent.h"
#include "Game/ShooterJam.h"
#include "GameModes/ShooterGameMode.h"
#include "PlayerControllers/ShooterCharacterController.h"
#include "PlayerState/ShooterPlayerState.h"

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

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;

	UCharacterMovementComponent* MovementComponent { GetCharacterMovement() };
	if(!MovementComponent)
		return;

	MovementComponent->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	if (CombatComponent)
	{
		CombatComponent->SetIsReplicated(true);
	}

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

	TurningInPlace = ETurningInPlace::TIP_NotTurning;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 720.f);

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	HudUpdateHealth();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AShooterCharacter::OnReceiveDamage);
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if(!PlayerController)
		return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if(!Subsystem) 
		return;

	Subsystem->AddMappingContext(InputMappingContext, 0);
}

void AShooterCharacter::OnMove(const FInputActionValue& Value)
{
	if (!GetController())
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
	Jump();
}

void AShooterCharacter::OnEquip(const FInputActionValue& Value)
{
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
	ActionAimStart();

	if (!HasAuthority())
	{
		Server_OnAimStart();
	}
}

void AShooterCharacter::OnAimEnd(const FInputActionValue& Value)
{
	ActionAimEnd();

	if (!HasAuthority())
	{
		Server_OnAimEnd();
	}
}

void AShooterCharacter::OnFireStart(const FInputActionValue& Value)
{
	if (!CombatComponent)
		return;

	CombatComponent->SetIsFiring(true);
}

void AShooterCharacter::OnFireEnd(const FInputActionValue& Value)
{
	if (!CombatComponent)
		return;

	CombatComponent->SetIsFiring(false);
}

//Received only on the server. Clients receive damage as replication of Health variable. See OnRep_Health
void AShooterCharacter::OnReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageTypem, class AController* InstigatorController, AActor* DamageCauser)
{
	//UE_LOG(LogTemp, Warning, TEXT("Damage received"))

	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);

	//That for server to play player-server effects
	if (HasAuthority())
	{
		ActionReceiveDamage();
	}

	if (Health > 0.f)
		return;

	AShooterGameMode* GameMode = GetShooterGameMode();
	if (!GameMode)
		return;

	AShooterCharacterController* AttackerController = Cast<AShooterCharacterController>(InstigatorController);
	if (!AttackerController)
		return;

	GameMode->OnPlayerEliminated(this, CharacterController, AttackerController);
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

	//if (!IsLocallyControlled())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Yaw: %f"), AO_Yaw);
	//}
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

	//UE_LOG(LogTemp, Warning, TEXT("AO_YAW: %f"), AO_Yaw);
	//UE_LOG(LogTemp, Warning, TEXT("Root AO_YAW: %f"), AO_Yaw);
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

void AShooterCharacter::PollInit()
{
	if (PlayerState)
		return;

	PlayerState = GetPlayerState<AShooterPlayerState>();
	if (!PlayerState)
		return;

	PlayerState->UpdateScoreHud();
	PlayerState->UpdateDefeatsHud();
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

	//UE_LOG(LogTemp, Warning, TEXT("ProxyYaw: %f"), ProxyYaw);

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

void AShooterCharacter::OnRep_Health()
{
	//UE_LOG(LogTemp, Warning, TEXT("Replication works"));
	ActionReceiveDamage();
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

void AShooterCharacter::Multicast_OnEliminated_Implementation()
{
	bIsEliminated = true;
	PlayEliminationMontage();

	PlayDissolvingEffect();
	PlayElimbotEffect();

	DisableCharacter();
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
	HudUpdateHealth();
}

void AShooterCharacter::DisableCharacter()
{
	//Disable movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	if (CharacterController)
	{
		DisableInput(CharacterController);
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

bool AShooterCharacter::GetIsWeaponEquipped() const
{
	return (CombatComponent && CombatComponent->GetIsWeaponEquipped());
}

bool AShooterCharacter::GetIsCrouched() const
{
	return bIsCrouched;
}

bool AShooterCharacter::GetIsAiming() const
{
	return (CombatComponent && CombatComponent->GetIsAiming());
}

float AShooterCharacter::GetAoYaw() const
{
	return AO_Yaw;
}

float AShooterCharacter::GetAoPitch() const
{
	return AO_Pitch;
}

float AShooterCharacter::GetHealth() const
{
	return Health;
}

float AShooterCharacter::GetMaxHealth() const
{
	return MaxHealth;
}

bool AShooterCharacter::GetRotateRootBone() const
{
	return bRotateRootBone;
}

bool AShooterCharacter::GetIsEliminated() const
{
	return bIsEliminated;
}

ETurningInPlace AShooterCharacter::GetTurningInPlace() const
{
	return TurningInPlace;
}

AWeaponBase* AShooterCharacter::GetEquippedWeapon() const
{
	if (!CombatComponent)
		return nullptr;

	return CombatComponent->GetEquippedWeapon();
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
	if (!CharacterController)
	{
		CharacterController = Cast<AShooterCharacterController>(GetController());
	}

	if (!CharacterController)
		return;

	CharacterController->SetHudHealth(Health, MaxHealth);
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

	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
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

	CheckHidePlayerIfCameraClose();
	PollInit();
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
}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AShooterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AShooterCharacter, Health);
}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (!CombatComponent)
		return;

	CombatComponent->Character = this;
	CombatComponent->PrimaryComponentTick.bCanEverTick = true;
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

void AShooterCharacter::Destroyed()
{
	Super::Destroy();

	if (!ElimbotComponent)
		return;

	ElimbotComponent->DestroyComponent();
}

