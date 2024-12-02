// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weaponry/WeaponBase.h"
#include "Components/CombatComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

	CombatComponent = CreateEditorOnlyDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

	TurningInPlace = ETurningInPlace::TIP_NotTurning;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 720.f);
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

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

void AShooterCharacter::CalculateAimOffset(float DeltaTime)
{
	if (!CombatComponent)
		return;

	if (!CombatComponent->GetIsWeaponEquipped())
		return;

	FVector CharacterVelocity{ GetVelocity() };
	CharacterVelocity.Z = 0.f;
	float Speed{ static_cast<float>(CharacterVelocity.Size()) };
	bool bIsInAir{ GetCharacterMovement()->IsFalling() };

	if (Speed == 0.f && !bIsInAir) //Standing still and not jumping
	{
		FRotator CurrentAimRotation{ FRotator(0.f, GetBaseAimRotation().Yaw, 0.f) };
		FRotator DeltaAimRotation{ UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation) };
		AO_Yaw = DeltaAimRotation.Yaw;
		bUseControllerRotationYaw = true;

		CalculateTurningInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) //Running or jumping
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;

		TurningInPlace = ETurningInPlace::TIP_NotTurning;
	}

	if (!IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("Yaw: %f"), AO_Yaw);
	}

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

	UE_LOG(LogTemp, Warning, TEXT("AO_YAW: %f"), AO_Yaw);
	UE_LOG(LogTemp, Warning, TEXT("Root AO_YAW: %f"), AO_Yaw);
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

bool AShooterCharacter::GetIsWeaponEquipped()
{
	return (CombatComponent && CombatComponent->GetIsWeaponEquipped());
}

bool AShooterCharacter::GetIsCrouched()
{
	return bIsCrouched;
}

bool AShooterCharacter::GetIsAiming()
{
	return (CombatComponent && CombatComponent->GetIsAiming());
}

float AShooterCharacter::GetAoYaw()
{
	return AO_Yaw;
}

float AShooterCharacter::GetAoPitch()
{
	return AO_Pitch;
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

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateAimOffset(DeltaTime);
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
}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AShooterCharacter, OverlappingWeapon, COND_OwnerOnly)
}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (!CombatComponent)
		return;

	CombatComponent->Character = this;
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

