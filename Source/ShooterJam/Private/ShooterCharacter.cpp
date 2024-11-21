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

}

void AShooterCharacter::OnEquip(const FInputActionValue& Value)
{
	if (!CombatComponent)
		return;

	if (HasAuthority())
	{
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}
	else
	{
		Server_EquipButtonPressed();
	}
}

void AShooterCharacter::OnRep_OverlappingWeapon(AWeaponBase* LastOverlappedWeapon)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString("Something overlaps"));

	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(true);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString("Showing widget"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString("No overlapping weapon"));
	}

	if (LastOverlappedWeapon && !OverlappingWeapon)
	{
		LastOverlappedWeapon->ShowPickUpWidget(false);
	}

}

void AShooterCharacter::Server_EquipButtonPressed_Implementation()
{
	if (!CombatComponent)
		return;

	CombatComponent->EquipWeapon(OverlappingWeapon);
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

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

