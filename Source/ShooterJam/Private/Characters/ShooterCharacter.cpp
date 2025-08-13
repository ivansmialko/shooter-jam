// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ShooterCharacter.h"

#include "Weaponry/WeaponBase.h"
#include "Animations/ShooterCharacterAnimInstance.h"
#include "Components/CombatComponent.h"
#include "Components/BuffComponent.h"
#include "Components/LagCompensationComponent.h"
#include "Game/ShooterJam.h"
#include "GameModes/ShooterGameMode.h"
#include "GameStates/ShooterGameState.h"
#include "PlayerControllers/ShooterCharacterController.h"
#include "PlayerState/ShooterPlayerState.h"
#include "Weaponry/WeaponTypes.h"
#include "HUD/WorldChat.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

AShooterCharacter::AShooterCharacter()
{
	InitializeCharacter();
	InitializeCameraBoom();
	InitializeFollowCamera();
	InitializeOverheadWidget();
	InitializeCombatComponent();
	InitializeBuffComponent();
	InitializeLagCompensationComponent();
	InitializeMovementComponent();
	InitializeMesh();
	InitializeGrenadeMesh();
	InitializeSsrHitBoxes();

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetGrenadeVisibility(false);
	SpawnDefaultWeapon();
	
	if (!HasAuthority())
		return;

	OnTakeAnyDamage.AddDynamic(this, &AShooterCharacter::OnReceiveDamage);
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

	if (CombatComponent && CombatComponent->GetIsDancing())
	{
		Server_StartEmotion("");
	}

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

	if (!CombatComponent)
		return;

	if (!CombatComponent->GetIsUnoccupied())
		return;

	UE_LOG(LogTemp, Warning, TEXT("[%s] Equip pressed"), (GetLocalRole() == ENetRole::ROLE_Authority ? TEXT("Server") : TEXT("Client")));

	Server_OnEquip();

	if (!CombatComponent->GetIsShouldSwapWeapons())
		return;

	if (HasAuthority())
		return;

	if (OverlappingWeapon)
		return;

	PlaySwapMontage();
	CombatComponent->ChangeCombatState(ECombatState::ECS_Swapping);
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
	if (!ShooterCharacterController)
		return;

	if (!ShooterCharacterController->GetPlayerHud())
		return;

	ShooterCharacterController->GetPlayerHud()->SetWeaponAmmoEmpty();
	ShooterCharacterController->GetPlayerHud()->SetCarriedAmmoEmpty();

	Server_OnDrop();
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
	RequestThrow();
}

void AShooterCharacter::OnEmotionPickerStarted(const FInputActionValue& Value)
{
	if (!ShooterCharacterController)
		return;

	ShooterCharacterController->GetPlayerHud()->ShowEmotionPickerWidget();
}

void AShooterCharacter::OnEmotionPickerCompleted(const FInputActionValue& Value)
{
	if (!ShooterCharacterController)
		return;

	ShooterCharacterController->GetPlayerHud()->HideEmotionPickerWidget();
}

void AShooterCharacter::OnEmotion1(const FInputActionValue& Value)
{
	if (!CombatComponent)
		return;

	if (!ShooterCharacterController)
		return;

	if (!ShooterCharacterController->GetPlayerHud()->GetIsEmotionPickerOpen())
		return;

	Server_StartEmotion(CombatComponent->GetIsDancing() ? "" : "Dance_Swing");
}

void AShooterCharacter::OnEmotion2(const FInputActionValue& Value)
{
	if (!CombatComponent)
		return;

	if (!ShooterCharacterController)
		return;

	if (!ShooterCharacterController->GetPlayerHud()->GetIsEmotionPickerOpen())
		return;

	Server_StartEmotion(CombatComponent->GetIsDancing() ? "" : "Dance_House");
}

void AShooterCharacter::OnEmotion3(const FInputActionValue& Value)
{
	if (!CombatComponent)
		return;

	if (!ShooterCharacterController)
		return;

	if (!ShooterCharacterController->GetPlayerHud()->GetIsEmotionPickerOpen())
		return;

	Server_StartEmotion(CombatComponent->GetIsDancing() ? "" : "Dance_Samba");
}

void AShooterCharacter::OnEmotion4(const FInputActionValue& Value)
{
	if (!CombatComponent)
		return;

	if (!ShooterCharacterController)
		return;

	if (!ShooterCharacterController->GetPlayerHud()->GetIsEmotionPickerOpen())
		return;

	Server_StartEmotion(CombatComponent->GetIsDancing() ? "" : "Dance_Arms");
}

//Received only on the server. Clients receive damage as replication of Health variable. See OnRep_Health
void AShooterCharacter::OnReceiveDamage(AActor* DamagedActor, float BaseDamage, const UDamageType* DamageTypem, class AController* InstigatorController, AActor* DamageCauser)
{
	if (bIsEliminated)
		return;

	if (!GetShooterGameMode())
		return;

	BaseDamage = GetShooterGameMode()->CalculateDamage(InstigatorController, Controller, BaseDamage);
	float DamageToHealth = BaseDamage;
	if (Shield > 0.f)
	{
		if (Shield >= BaseDamage)
		{
			Shield = FMath::Clamp(Shield - BaseDamage, 0, MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0, BaseDamage);
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

	AShooterCharacterController* AttackerController = Cast<AShooterCharacterController>(InstigatorController);
	if (!AttackerController)
		return;

	GetShooterGameMode()->OnPlayerEliminated(this, ShooterCharacterController, AttackerController);
}

void AShooterCharacter::OnPingTooHigh(bool bInIsTooHighPing)
{
	if (!CombatComponent)
		return;

	CombatComponent->SetWeaponsUseSsr(!bInIsTooHighPing);
}

void AShooterCharacter::OnEliminatedTimerFinished()
{
	AShooterGameMode* GameMode = GetShooterGameMode();
	if (GameMode && !bLeftGame)
	{
		GameMode->RequestRespawn(this, GetController());
	}

	if (bLeftGame && IsLocallyControlled())
	{
		OnLeftGame.Broadcast();
	}
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

void AShooterCharacter::InitializeSsrHitBoxes()
{
	/** Hit boxes for server-side rewind */
	SsrHead = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_Head"));
	SsrHead->SetupAttachment(GetMesh(), FName("head"));
	SsrCollisionBoxes.Add(FName("head"), SsrHead);

	SsrPelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_Pelvis"));
	SsrPelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	SsrCollisionBoxes.Add(FName("pelvis"), SsrPelvis);

	SsrSpine02 = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_Spine02"));
	SsrSpine02->SetupAttachment(GetMesh(), FName("spine_02"));
	SsrCollisionBoxes.Add(FName("spine_02"), SsrSpine02);

	SsrSpine03 = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_Spine03"));
	SsrSpine03->SetupAttachment(GetMesh(), FName("spine_03"));
	SsrCollisionBoxes.Add(FName("spine_03"), SsrSpine03);

	SsrUpperArmL = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_UpperArmL"));
	SsrUpperArmL->SetupAttachment(GetMesh(), FName("upperarm_l"));
	SsrCollisionBoxes.Add(FName("upperarm_l"), SsrUpperArmL);

	SsrUpperArmR = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_UpperArmR"));
	SsrUpperArmR->SetupAttachment(GetMesh(), FName("upperarm_r"));
	SsrCollisionBoxes.Add(FName("upperarm_r"), SsrUpperArmR);

	SsrLowerArmL = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_LowerArmL"));
	SsrLowerArmL->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	SsrCollisionBoxes.Add(FName("lowerarm_l"), SsrLowerArmL);

	SsrLowerArmR = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_LowerArmR"));
	SsrLowerArmR->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	SsrCollisionBoxes.Add(FName("lowerarm_r"), SsrLowerArmR);

	SsrHandL = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_HandL"));
	SsrHandL->SetupAttachment(GetMesh(), FName("hand_l"));
	SsrCollisionBoxes.Add(FName("hand_l"), SsrHandL);

	SsrHandR = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_HandR"));
	SsrHandR->SetupAttachment(GetMesh(), FName("hand_r"));
	SsrCollisionBoxes.Add(FName("hand_r"), SsrHandR);

	SsrBackpack = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_Backpack"));
	SsrBackpack->SetupAttachment(GetMesh(), FName("backpack"));
	SsrCollisionBoxes.Add(FName("backpack"), SsrBackpack);

	SsrBackpackBlanket = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_BackpackBlanket"));
	SsrBackpackBlanket->SetupAttachment(GetMesh(), FName("blanket"));
	SsrCollisionBoxes.Add(FName("blanket"), SsrBackpackBlanket);

	SsrThighL = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_ThighL"));
	SsrThighL->SetupAttachment(GetMesh(), FName("thigh_l"));
	SsrCollisionBoxes.Add(FName("thigh_l"), SsrThighL);

	SsrThighR = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_ThighR"));
	SsrThighR->SetupAttachment(GetMesh(), FName("thigh_r"));
	SsrCollisionBoxes.Add(FName("thigh_r"), SsrThighR);

	SsrCalfL = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_CalfL"));
	SsrCalfL->SetupAttachment(GetMesh(), FName("calf_l"));
	SsrCollisionBoxes.Add(FName("calf_l"), SsrCalfL);

	SsrCalrR = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_CalfR"));
	SsrCalrR->SetupAttachment(GetMesh(), FName("calf_r"));
	SsrCollisionBoxes.Add(FName("calf_r"), SsrCalrR);

	SsrFootL = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_FootL"));
	SsrFootL->SetupAttachment(GetMesh(), FName("foot_l"));
	SsrCollisionBoxes.Add(FName("foot_l"), SsrFootL);

	SsrFootR = CreateDefaultSubobject<UBoxComponent>(TEXT("SSR_FootR"));
	SsrFootR->SetupAttachment(GetMesh(), FName("foot_r"));
	SsrCollisionBoxes.Add(FName("foot_r"), SsrFootR);

	for (const auto& [BoxName, Box] : SsrCollisionBoxes)
	{
		Box->SetCollisionObjectType(ECC_HitBox);
		Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Box->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AShooterCharacter::InitializeCameraBoom()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	if (!CameraBoom)
		return;

	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoomDefaultOffset = CameraBoom->SocketOffset;
}

void AShooterCharacter::InitializeFollowCamera()
{
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	if (!FollowCamera)
		return;

	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void AShooterCharacter::InitializeOverheadWidget()
{
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	if (!OverheadWidget)
		return;

	OverheadWidget->SetupAttachment(RootComponent);
}

void AShooterCharacter::InitializeCombatComponent()
{
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	if (!CombatComponent)
		return;

	CombatComponent->SetIsReplicated(true);
}

void AShooterCharacter::InitializeBuffComponent()
{
	BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	if (!BuffComponent)
		return;

	BuffComponent->SetIsReplicated(true);
}

void AShooterCharacter::InitializeLagCompensationComponent()
{
	LagCompensationComponent = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensationComponent"));
}

void AShooterCharacter::InitializeMovementComponent()
{
	UCharacterMovementComponent* MovementComponent{ GetCharacterMovement() };
	if (!MovementComponent)
		return;

	MovementComponent->bOrientRotationToMovement = true;
	MovementComponent->NavAgentProps.bCanCrouch = true;
	MovementComponent->MaxWalkSpeed = BaseWalkSpeed;
	MovementComponent->RotationRate = FRotator(0.f, 0.f, 720.f);
}

void AShooterCharacter::InitializeMesh()
{
	USkeletalMeshComponent* SkeletalMesh{ GetMesh() };
	if (!SkeletalMesh)
		return;

	SkeletalMesh->SetCollisionObjectType(ECC_SkeletalMesh);
	SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}

void AShooterCharacter::InitializeGrenadeMesh()
{
	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	if (!GrenadeMesh)
		return;

	GrenadeMesh->SetupAttachment(GetMesh(), FName("RightHandGrenadeSocket"));
	GrenadeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AShooterCharacter::InitializeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	SetNetUpdateFrequency(66.f);
	SetMinNetUpdateFrequency(33.f);
}

void AShooterCharacter::PollInitPlayerState()
{
	if (ShooterPlayerState)
		return;

	ShooterPlayerState = GetPlayerState<AShooterPlayerState>();
	if (!ShooterPlayerState)
		return;

	ShooterPlayerState->UpdateScoreHud();
	ShooterPlayerState->UpdateDefeatsHud();

	CheckShowCrown();
	ChangeTeamType(ShooterPlayerState->GetTeamType());
}

void AShooterCharacter::PollInitPlayerController()
{
	if (ShooterCharacterController)
		return;

	ShooterCharacterController = GetController<AShooterCharacterController>();

	if (!ShooterCharacterController)
		return;

	if (!ShooterCharacterController->OnTooHighPingDelegate.IsBound() && HasAuthority())
	{
		ShooterCharacterController->OnTooHighPingDelegate.AddDynamic(this, &AShooterCharacter::OnPingTooHigh);
	}
}

void AShooterCharacter::PollInitPlayerHud()
{
	if (bHudInitialized)
		return;

	if (!ShooterCharacterController)
		return;

	if (!ShooterCharacterController->GetPlayerHud())
		return;

	if (!ShooterCharacterController->GetPlayerHud()->IsOverlayInitialized())
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

	if (!CombatComponent->GetSecondaryWeapon())
		return;

	CombatComponent->GetSecondaryWeapon()->GetWeaponMesh()->bOwnerNoSee = (DistanceFromCameraToCharacter < CameraDistanceTreshold);
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

void AShooterCharacter::OnRep_CurrentEmotion()
{
	if (CurrentEmotion.IsNone())
	{
		ActionStopEmotion();
	}
	else
	{
		ActionStartEmotion(CurrentEmotion);
	}
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

void AShooterCharacter::Server_OnDrop_Implementation()
{
	ActionDrop();
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

void AShooterCharacter::Server_LeaveGame_Implementation()
{
	UWorld* World{ GetWorld() };
	if (!World)
		return;

	AShooterGameMode* GameMode{ World->GetAuthGameMode<AShooterGameMode>() };
	if (!GameMode)
		return;

	if (!GetShooterPlayerState())
		return;

	GameMode->OnPlayerLeft(GetShooterPlayerState());
}

void AShooterCharacter::Multicast_GainedLead_Implementation()
{
	if (!CrownEffect)
		return;

	if (!CrownEffectComponent)
	{
		CrownEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			CrownEffect,
			GetMesh(),
			FName(),
			GetActorLocation() + FVector(0.f, 0.f, 110.f),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
	 
	if (!CrownEffectComponent)
		return;

	CrownEffectComponent->Activate();
}

void AShooterCharacter::Multicast_LostLead_Implementation()
{
	if (!CrownEffectComponent)
		return;

	CrownEffectComponent->DestroyComponent();
}

void AShooterCharacter::Multicast_OnEliminated_Implementation(bool bInLeftGame)
{
	bIsEliminated = true;
	bLeftGame = bInLeftGame;
	PlayEliminationMontage();

	PlayDissolvingEffect();
	PlayElimbotEffect();

	DisableInputs();

	GetWorldTimerManager().SetTimer(EliminatedTimer, this, &AShooterCharacter::OnEliminatedTimerFinished, EliminationDelay);

	if (GrenadeMesh)
	{
		GrenadeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (!ShooterCharacterController)
		return;

	if (!ShooterCharacterController->GetPlayerHud())
		return;

	ShooterCharacterController->GetPlayerHud()->SetWeaponAmmo(0);
}

void AShooterCharacter::Server_StartEmotion_Implementation(FName EmotionName)
{
	CurrentEmotion = EmotionName;

	if (!HasAuthority())
		return;

	if (EmotionName.IsNone())
	{
		ActionStopEmotion();
	}
	else
	{
		ActionStartEmotion(EmotionName);
	}
}

void AShooterCharacter::ActionEquip()
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] Action equip"), (GetLocalRole() == ENetRole::ROLE_Authority ? TEXT("Server") : TEXT("Client")));

	if (!CombatComponent)
		return;

	if (OverlappingWeapon)
	{
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}
	else if (CombatComponent->GetIsShouldSwapWeapons())
	{
		CombatComponent->SwapWeapons();
	}
}

void AShooterCharacter::ActionDrop()
{
	if (!CombatComponent)
		return;

	CombatComponent->DropWeaponLaunch();
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

void AShooterCharacter::RequestThrow()
{
	if (!CombatComponent)
		return;

	CombatComponent->Throw();
}

void AShooterCharacter::ActionStartEmotion(FName EmotionName)
{
	if (!CombatComponent)
		return;

	CombatComponent->StartDancing();
	DisableGameplay();

	if (!DancingMusic.Contains(EmotionName))
		return;

	EmotionAudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, DancingMusic[EmotionName], GetActorLocation());
	if (!EmotionAudioComponent)
		return;

	EmotionAudioComponent->OnAudioFinished.AddDynamic(this, &AShooterCharacter::OnEmotionSoundFinished);
	EmotionAudioComponent->SetIsReplicated(true);
}

void AShooterCharacter::ActionStopEmotion()
{
	if (!CombatComponent)
		return;

	CombatComponent->StopDancing();
	EnableGameplay();

	if (EmotionAudioComponent)
	{
		EmotionAudioComponent->Stop();
		EmotionAudioComponent->DestroyComponent();
	}
}

void AShooterCharacter::DisableInputs()
{
	//Disable movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	if (ShooterCharacterController)
	{
		DisableInput(ShooterCharacterController);
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

	if (CombatComponent->GetEquippedWeapon()->GetIsDestroyAfterDeath())
	{
		CombatComponent->GetEquippedWeapon()->Destroy();
		return;
	}

	CombatComponent->GetEquippedWeapon()->OnDropped();
}

void AShooterCharacter::DropSecondaryWeapon()
{
	if (!CombatComponent)
		return;

	if (!CombatComponent->GetSecondaryWeapon())
		return;

	if (CombatComponent->GetSecondaryWeapon()->GetIsDestroyAfterDeath())
	{
		CombatComponent->GetSecondaryWeapon()->Destroy();
		return;
	}

	CombatComponent->GetSecondaryWeapon()->OnDropped();
}

void AShooterCharacter::CheckShowCrown()
{
	if (!GetShooterPlayerState())
		return;

	AShooterGameState* ShooterGameState{ Cast<AShooterGameState>(UGameplayStatics::GetGameState(this)) };
	if (!ShooterGameState)
		return;

	if (!ShooterGameState->IsPlayerLeading(GetShooterPlayerState()))
		return;

	Multicast_GainedLead();
}

void AShooterCharacter::ChangeTeamType(ETeamType InTeamType)
{
	if (!GetMesh())
		return;

	if (!PlayerSkinDefault)
		return;

	switch (InTeamType)
	{
	case ETeamType::ETT_Red:
	{
		GetMesh()->SetMaterial(0, PlayerSkinBlue);
		DissolveMaterialInstance = DissolveMaterialBlueInstance;
	} break;
	case ETeamType::ETT_Blue:
	{
		GetMesh()->SetMaterial(0, PlayerSkinRed);
		DissolveMaterialInstance = DissolveMaterialRedInstance;
	} break;
	default:
	{
		GetMesh()->SetMaterial(0, PlayerSkinDefault);
		DissolveMaterialInstance = DissolveMaterialBlueInstance;
	} break;
	}
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

void AShooterCharacter::EnableGameplay()
{
	bGameplayEnabled = true;
	CombatComponent->SetIsAiming(false);
	CombatComponent->SetIsFiring(false);
	bUseControllerRotationYaw = true;
	TurningInPlace = ETurningInPlace::TIP_NotTurning;
}

void AShooterCharacter::HudUpdate()
{
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

void AShooterCharacter::PostInitializeLagCompensationComponent()
{
	if (!LagCompensationComponent)
		return;

	LagCompensationComponent->SetCharacter(this);
	LagCompensationComponent->SetController(Cast<AShooterCharacterController>(GetController()));
}

void AShooterCharacter::SpawnDefaultWeapon()
{
	if (bIsEliminated)
		return;

	if (!DefaultWeaponClass)
		return;

	UWorld* World = GetWorld();
	if (!World)
		return;

	AWeaponBase* StartingWeapon = World->SpawnActor<AWeaponBase>(DefaultWeaponClass);
	if (!StartingWeapon)
		return;

	StartingWeapon->SetIsDestroyAfterDeath(true);

	if (!CombatComponent)
		return;

	CombatComponent->EquipWeapon(StartingWeapon);

}

FName AShooterCharacter::GetRandomDancingAnimation() const
{
	if (!DancingMontage)
		return FName();

	if (DancingMontage->GetNumSections() == 0)
		return FName();

	return DancingMontage->GetSectionName(FMath::RandRange(0, DancingMontage->GetNumSections() - 1));
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

void AShooterCharacter::PlaySwapMontage()
{
	if (!GetMesh())
		return;

	if (!SwapMontage)
		return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
		return;

	AnimInstance->Montage_Play(SwapMontage);
}

void AShooterCharacter::PlayDancingMontage()
{
	if (!GetMesh())
		return;

	if (!DancingMontage)
		return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
		return;

	AnimInstance->Montage_Play(DancingMontage);
	AnimInstance->Montage_JumpToSection(CurrentEmotion);
}

void AShooterCharacter::StopDancingMontage()
{
	if (!GetMesh())
		return;

	if (!DancingMontage)
		return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
		return;

	AnimInstance->Montage_Stop(0.1f, DancingMontage);
}

void AShooterCharacter::OnAnimReloadFinished()
{
	if (!CombatComponent)
		return;

	CombatComponent->OnAnimReloadFinished();
}

void AShooterCharacter::OnAnimShellInserted()
{
	if (!CombatComponent)
		return;

	CombatComponent->OnAnimShellInserted();
}

void AShooterCharacter::OnAnimThrowFinished()
{
	if (!CombatComponent)
		return;

	CombatComponent->OnAnimThrowFinished();
}

void AShooterCharacter::OnAnimThrowLaunched()
{
	if (!CombatComponent)
		return;

	CombatComponent->OnAnimThrowLaunched();
}

void AShooterCharacter::OnAnimSwapSwapped()
{
	if (!CombatComponent)
		return;

	//UE_LOG(LogTemp, Warning, TEXT("[%s][%s] Playing swapped anim notify %s"),
	//	(GetLocalRole() == ENetRole::ROLE_Authority ? TEXT("Server") : TEXT("Client")),
	//	(IsLocallyControlled() ? TEXT("Local") : TEXT("Remote")),
	//	(*GetActorLabel()));

	CombatComponent->OnAnimSwapSwapped();
}

void AShooterCharacter::OnAnimSwapFinished()
{
	if (!CombatComponent)
		return;

	CombatComponent->OnAnimSwapFinished();
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
	if (!ShooterCharacterController)
	{
		ShooterCharacterController = Cast<AShooterCharacterController>(GetController());
	}

	if (!ShooterCharacterController)
		return;

	if (!ShooterCharacterController->GetPlayerHud())
		return;

	ShooterCharacterController->GetPlayerHud()->SetHealth(Health, MaxHealth);
}

void AShooterCharacter::HudUpdateShield()
{
	if (!ShooterCharacterController)
	{
		ShooterCharacterController = Cast<AShooterCharacterController>(GetController());
	}

	if (!ShooterCharacterController)
		return;

	if (!ShooterCharacterController->GetPlayerHud())
		return;

	ShooterCharacterController->GetPlayerHud()->SetShield(Shield, MaxShield);
}

void AShooterCharacter::HudUpdateAmmo()
{
	if (!ShooterCharacterController)
	{
		ShooterCharacterController = Cast<AShooterCharacterController>(GetController());
	}

	if (!ShooterCharacterController)
		return;

	if (!ShooterCharacterController->GetPlayerHud())
		return;

	if (!CombatComponent)
		return;

	if (!CombatComponent->GetEquippedWeapon())
	{
		ShooterCharacterController->GetPlayerHud()->SetWeaponAmmoEmpty();
		ShooterCharacterController->GetPlayerHud()->SetCarriedAmmoEmpty();
		return;
	}

	ShooterCharacterController->GetPlayerHud()->SetWeaponAmmo(CombatComponent->GetWeaponAmmo());
	ShooterCharacterController->GetPlayerHud()->SetCarriedAmmo(CombatComponent->GetCarriedAmmo());
}

void AShooterCharacter::HudUpdateGrenades()
{
	if (!ShooterCharacterController)
	{
		ShooterCharacterController = Cast<AShooterCharacterController>(GetController());
	}

	if (!ShooterCharacterController)
		return;

	if (!ShooterCharacterController->GetPlayerHud())
		return;

	if (!CombatComponent)
		return;

	ShooterCharacterController->GetPlayerHud()->SetGrenadesAmount(CombatComponent->GetGrenadesAmount());
}

void AShooterCharacter::TimelineUpdateDissolveMaterial(float InDissolveValue)
{
	if (!DissolveMaterialInstanceDynamic)
		return;

	DissolveMaterialInstanceDynamic->SetScalarParameterValue(TEXT("Dissolve"), InDissolveValue);
}

void AShooterCharacter::OnEmotionSoundFinished()
{
	if (!EmotionAudioComponent)
		return;

	Server_StartEmotion("");
}

AShooterGameMode* AShooterCharacter::GetShooterGameMode()
{
	if (ShooterGameMode)
		return ShooterGameMode;

	ShooterGameMode = GetWorld()->GetAuthGameMode<AShooterGameMode>();
	return ShooterGameMode;
}

AShooterPlayerState* AShooterCharacter::GetShooterPlayerState()
{
	if (ShooterPlayerState)
		return ShooterPlayerState;

	ShooterPlayerState = GetPlayerState<AShooterPlayerState>();
	return ShooterPlayerState;
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
	EnhancedInput->BindAction(Emotion1Action, ETriggerEvent::Started, this, &AShooterCharacter::OnEmotion1);
	EnhancedInput->BindAction(Emotion2Action, ETriggerEvent::Started, this, &AShooterCharacter::OnEmotion2);
	EnhancedInput->BindAction(Emotion3Action, ETriggerEvent::Started, this, &AShooterCharacter::OnEmotion3);
	EnhancedInput->BindAction(Emotion4Action, ETriggerEvent::Started, this, &AShooterCharacter::OnEmotion4);
	EnhancedInput->BindAction(EmotionPickerAction, ETriggerEvent::Started, this, &AShooterCharacter::OnEmotionPickerStarted);
	EnhancedInput->BindAction(EmotionPickerAction, ETriggerEvent::Completed, this, &AShooterCharacter::OnEmotionPickerCompleted);
}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AShooterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AShooterCharacter, Health);
	DOREPLIFETIME(AShooterCharacter, Shield);
	DOREPLIFETIME(AShooterCharacter, bGameplayEnabled);
	DOREPLIFETIME(AShooterCharacter, CurrentEmotion);
}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	PostInitializeCombatComponent();
	PostInitializeBuffComponent();
	PostInitializeLagCompensationComponent();
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

void AShooterCharacter::OnEliminated(bool bInLeftGame)
{
	Multicast_OnEliminated(bInLeftGame);

	DropWeapon();
	DropSecondaryWeapon();
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

