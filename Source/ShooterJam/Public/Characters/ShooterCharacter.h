// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Animations/TurningInPlace.h"
#include "Interfaces/CrosshairsInteractable.h"
#include "Components/CombatComponent.h"
#include "PlayerState/ShooterPlayerState.h"

#include "Components/TimelineComponent.h"

#include "ShooterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class UAnimMontage;
class USoundCue;
class UInputMappingContext;
class UInputAction;
class UBoxComponent;
class UNiagaraSystem;
class UNiagaraComponent; 

class AShooterCharacterController;
class AShooterPlayerState;
class AShooterGameMode;
class AWeaponBase;
class UBuffComponent;
class ULagCompensationComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class SHOOTERJAM_API AShooterCharacter : public ACharacter, public ICrosshairsInteractable
{
	GENERATED_BODY()


//public members
public:
	FOnLeftGame OnLeftGame;

//private members
private:

//~ Begin Exposed members
	/** UMG widget class of user HUD */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* OverheadWidget;

	/** Animation montage of firing */
	UPROPERTY(EditAnywhere, Category = Animations)
	UAnimMontage* FireWeaponMontage;

	/** Animation montage of hitting with bullet */
	UPROPERTY(EditAnywhere, Category = Animations)
	UAnimMontage* HitReactMontage;

	/** Animation montage of hitting with bullet */
	UPROPERTY(EditAnywhere, Category = Animations)
	UAnimMontage* EliminationMontage;

	/** Animation montage of reloading */
	UPROPERTY(EditAnywhere, Category = Animations)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimMontage* ThrowMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimMontage* SwapMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimMontage* DancingMontage;

	/** Float curve that represents dissolving of character's mesh after dying */
	UPROPERTY(EditAnywhere, Category = Effects)
	UCurveFloat* DissolveCurve;

	/** Material instance of dissolve material. */
	UPROPERTY(EditAnywhere, Category = Effects)
	UMaterialInstance* DissolveMaterialInstance;				//Selected in the editor

	/** Material instance of dissolve material (red color) */
	UPROPERTY(EditAnywhere, Category = Effects)
	UMaterialInstance* DissolveMaterialRedInstance;				//Selected in the editor

	/** Material instance of dissolve material (blue color) */
	UPROPERTY(EditAnywhere, Category = Effects)
	UMaterialInstance* DissolveMaterialBlueInstance;			//Selected in the editor

	/** Material instance of red player's skin */
	UPROPERTY(EditAnywhere, Category = Effects)
	UMaterialInstance* PlayerSkinRed;

	/** Material instance of blue player's skin */
	UPROPERTY(EditAnywhere, Category = Effects)
	UMaterialInstance* PlayerSkinBlue;

	/** Material instance of blue player's skin */
	UPROPERTY(EditAnywhere, Category = Effects)
	UMaterialInstance* PlayerSkinDefault;

	/** Elimination bot particle system. */
	UPROPERTY(EditAnywhere, Category = Effects)
	UParticleSystem* ElimbotEffect;								//Selected in the editor

	UPROPERTY(EditAnywhere, Category = Effects)
	UNiagaraSystem* CrownEffect;								//Selected in the editor

	/** Sound of elimination. */
	UPROPERTY(EditAnywhere, Category = Effects)
	USoundCue* ElimbotSound;

	UPROPERTY(EditAnywhere, Category = Effects)
	TMap<FName, USoundCue*> DancingMusic;

	UPROPERTY(VisibleAnywhere, Category = Effects)
	UAudioComponent* EmotionAudioComponent;						//Currently playing emotion sound

	UPROPERTY(VisibleAnywhere, Category = Effects)
	UStaticMeshComponent* GrenadeMesh;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<AWeaponBase> DefaultWeaponClass;

	/** Delay between start of an dissolve animation and actual elimination */
	UPROPERTY(EditDefaultsOnly)
	float EliminationDelay{ 3.f };

	/** Usual walking speed */
	UPROPERTY(EditAnywhere, Category = Movement)
	float BaseWalkSpeed{ 600.f };

	/** Walking speed while aiming */
	UPROPERTY(EditAnywhere, Category = Movement)
	float AimWalkSpeed{ 450.f };

	/** Minimal distance between camera and mesh of a player. Below this value character mesh becoming invisible */
	UPROPERTY(EditAnywhere)
	double CameraDistanceTreshold{ 200.0 };

	/** Maximum possible amount of health. Used to calculate width of a progress bar */
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth{ 100.f };

	/** Maximum possible amount of shield. Used to calculate width of a progress bar */
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield{ 100.f };

	/** Current value of player's health */
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health{ MaxHealth };

	/** Current value of player's shield */
	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
	float Shield{ MaxShield };

	UPROPERTY(ReplicatedUsing = OnRep_CurrentEmotion, VisibleAnywhere);
	FName CurrentEmotion;

	/** Is user currently eliminated */
	UPROPERTY(VisibleAnywhere)
	bool bIsEliminated{ false };

	/** If true - all user inputs are disabled, except for mouse movement */
	UPROPERTY(VisibleAnywhere, Replicated)
	bool bGameplayEnabled{ true };

	//User input
	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EquipAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AimAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* FireAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* DropWeaponAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ReloadAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ThrowAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ExitAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* Emotion1Action;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* Emotion2Action;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* Emotion3Action;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* Emotion4Action;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EmotionPickerAction;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	UPROPERTY(VisibleAnywhere, Category = Effects)
	UMaterialInstanceDynamic* DissolveMaterialInstanceDynamic;	//Instance of UMaterialInstance that we create in runtime
	UPROPERTY(VisibleAnywhere, Category = Effects)
	UParticleSystemComponent* ElimbotComponent;					//Instance of Particle system that we spawn in runtime
	UPROPERTY(VisibleAnywhere, Category = Effects)
	UNiagaraComponent* CrownEffectComponent;

	UPROPERTY(VisibleAnywhere)
	UCombatComponent* CombatComponent;
	UPROPERTY(VisibleAnywhere)
	UBuffComponent* BuffComponent;
	UPROPERTY(VisibleAnywhere)
	ULagCompensationComponent* LagCompensationComponent;

	/** Hit boxes for server-side rewind */
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrHead;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrPelvis;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrSpine02;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrSpine03;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrUpperArmL;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrUpperArmR;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrLowerArmL;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrLowerArmR;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrHandL;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrHandR;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrBackpack;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrBackpackBlanket;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrThighL;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrThighR;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrCalfL;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrCalrR;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrFootL;
	UPROPERTY(EditAnywhere, Category = "Server side rewind")
	UBoxComponent* SsrFootR;
//~ End Exposed members

//~ Begin Replicated members
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeaponBase* OverlappingWeapon;
//~ End Replicated members

	bool bRotateRootBone;
	bool bInputInitialized{ false };
	bool bHudInitialized{ false };
	bool bLeftGame{ false };
	float AO_Yaw;
	float AO_Pitch;
	float Root_AO_Yaw;
	float ProxyTurnTreshold{ 0.5f };
	float ProxyYaw;
	float TimeSinceLastMovementRep{ 0.f };
	FRotator ProxyRotation;
	FRotator ProxyRotationLastFrame;
	FRotator StartingAimRotation;
	FVector CameraBoomDefaultOffset;
	
	ETurningInPlace TurningInPlace{ ETurningInPlace::TIP_NotTurning };
	
	FTimerHandle EliminatedTimer;
	FOnTimelineFloat DissolveTrackDlg;

	AShooterCharacterController* ShooterCharacterController{ nullptr };
	AShooterPlayerState* ShooterPlayerState{ nullptr };
	AShooterGameMode* ShooterGameMode{ nullptr };

	TMap<FName, UBoxComponent*> SsrCollisionBoxes;
//protected methods
protected:

//~ Begin AActor Interface
	virtual void BeginPlay() override;
//~ End AActor Interface

//~ Begin APawn Interface
	virtual void Restart() override;
//~ End APawn Interface

//private methods
private:
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeaponBase* LastOverlappedWeapon); //LastOverlappedWeapon is the last value of replicated variable, before it will be set
	UFUNCTION()
	void OnRep_Health(float LastHealth);
	UFUNCTION()
	void OnRep_Shield(float LastShield);
	UFUNCTION()
	void OnRep_CurrentEmotion();

	virtual void OnRep_ReplicatedMovement() override;
	UFUNCTION()
	void OnReceiveDamage(AActor* DamagedActor, float BaseDamage, const UDamageType* DamageTypem, class AController* InstigatorController, AActor* DamageCauser);
	UFUNCTION()
	void OnPingTooHigh(bool bInIsTooHighPing);
	void OnEliminatedTimerFinished();
	UFUNCTION()
	void TimelineUpdateDissolveMaterial(float InDissolveValue);
	UFUNCTION()
	void OnEmotionSoundFinished();

	UFUNCTION(Server, Reliable)
	void Server_OnEquip();
	UFUNCTION(Server, Reliable)
	void Server_OnAimStart();
	UFUNCTION(Server, Reliable)
	void Server_OnAimEnd();
	UFUNCTION(Server, Reliable)
	void Server_OnReload();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnEliminated(bool bInLeftGame);
	UFUNCTION(Server, Reliable)
	void Server_StartEmotion(FName EmotionName);

	void ActionEquip();
	void ActionAimStart();
	void ActionAimEnd();
	void ActionReceiveDamage();
	void ActionReload();
	void RequestThrow();
	void ActionStartEmotion(FName EmotionName);
	void ActionStopEmotion();

	void DropWeapon();
	void DropSecondaryWeapon();

	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnJump(const FInputActionValue& Value);
	void OnEquip(const FInputActionValue& Value);
	void OnCrouch(const FInputActionValue& Value);
	void OnAimStart(const FInputActionValue& Value);
	void OnAimEnd(const FInputActionValue& Value);
	void OnFireStart(const FInputActionValue& Value);
	void OnFireEnd(const FInputActionValue& Value);
	void OnDropWeapon(const FInputActionValue& Value);
	void OnReload(const FInputActionValue& Value);
	void OnThrow(const FInputActionValue& Value);
	void OnEmotionPickerStarted(const FInputActionValue& Value);
	void OnEmotionPickerCompleted(const FInputActionValue& Value);
	void OnEmotion1(const FInputActionValue& Value);
	void OnEmotion2(const FInputActionValue& Value);
	void OnEmotion3(const FInputActionValue& Value);
	void OnEmotion4(const FInputActionValue& Value);

	void CalculateAimOffset(float DeltaTime);
	void CalculateAimOffset_SimProxies();
	void CalculateAimPitch();
	void CalculateTurningInPlace(float DeltaTime);
	float CalculateSpeed() const;

	void InitializeSsrHitBoxes();
	void InitializeCameraBoom();
	void InitializeFollowCamera();
	void InitializeOverheadWidget();
	void InitializeCombatComponent();
	void InitializeBuffComponent();
	void InitializeLagCompensationComponent();
	void InitializeMovementComponent();
	void InitializeMesh();
	void InitializeGrenadeMesh();
	void InitializeCharacter();

	void PollInitPlayerState();
	void PollInitPlayerController();
	void PollInitPlayerHud();

	void CheckHidePlayerIfCameraClose();
	void PlayHitReactMontage();
	void PlayDissolvingEffect();
	void PlayElimbotEffect();
	void PlayReloadMontage(bool bInPlayReloadEnd);

	void PostInitializeCombatComponent();
	void PostInitializeBuffComponent();
	void PostInitializeLagCompensationComponent();

	void SpawnDefaultWeapon();

	FName GetRandomDancingAnimation() const;

//public methods
public:
	AShooterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void Jump() override;
	virtual void Destroyed() override;

	UFUNCTION(Server, Reliable)
	void Server_LeaveGame();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GainedLead();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_LostLead();

	virtual void OnEliminated(bool bInLeftGame);
	virtual void OnSpendRound(AWeaponBase* InWeapon);

	virtual void OnAnimReloadFinished();
	virtual void OnAnimShellInserted();
	virtual void OnAnimThrowFinished();
	virtual void OnAnimThrowLaunched();
	virtual void OnAnimSwapSwapped();
	virtual void OnAnimSwapFinished();

	void PlayFireMontage(bool bInIsAiming);
	void PlayEliminationMontage();
	void PlayReloadMontage();
	void PlayReloadEndMontage();
	void PlayThrowMontage();
	void PlaySwapMontage();
	void PlayDancingMontage();
	void StopDancingMontage();

	void InitInputs();
	void DisableInputs();
	void DisableGameplay();
	void EnableGameplay();

	void HudUpdate();
	void HudUpdateHealth();
	void HudUpdateShield();
	void HudUpdateAmmo();
	void HudUpdateGrenades();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bInShowScope);

	void CheckShowCrown();
	void ChangeTeamType(ETeamType InTeamType);

	//public getters/setters
public:

	void SetOverlappingWeapon(AWeaponBase* Weapon);
	void SetGrenadeVisibility(bool bVisible);
	void SetWalkSpeed(const float InWalkSpeed);
	void SetCrouchSpeed(const float InCrouchSpeed);
	void SetJumpVelocity(const float InJumpVelocity);

	FORCEINLINE bool GetIsWeaponEquipped() const { return (CombatComponent && CombatComponent->GetIsWeaponEquipped()); }
	FORCEINLINE bool GetIsCrouched() const { return bIsCrouched; }
	FORCEINLINE bool GetIsAiming() const { return (CombatComponent && CombatComponent->GetIsAiming()); }
	FORCEINLINE bool GetRotateRootBone() const { return bRotateRootBone; };
	FORCEINLINE bool GetIsEliminated() const { return bIsEliminated; };
	FORCEINLINE bool GetIsReloading() const { return (CombatComponent && CombatComponent->GetIsReloading()); }
	FORCEINLINE bool GetIsThrowing() const { return (CombatComponent && CombatComponent->GetIsThrowing()); }
	FORCEINLINE bool GetIsUnoccupied() const { return (CombatComponent && CombatComponent->GetIsUnoccupied()); }
	FORCEINLINE bool GetIsGameplayEnabled() const { return bGameplayEnabled; }
	FORCEINLINE bool GetUseFabrik() const { return GetIsUnoccupied(); }
	FORCEINLINE bool GetUseAimOffsets() const { return GetIsUnoccupied(); }
	FORCEINLINE bool GetUseRightHandTransform() const { return GetIsUnoccupied(); }
	FORCEINLINE bool GetIsLeftGame() const { return bLeftGame; }
	FORCEINLINE bool GetIsDancing() const { return (CombatComponent && CombatComponent->GetIsDancing()); }
	FORCEINLINE float GetAoYaw() const { return AO_Yaw; };
	FORCEINLINE float GetAoPitch() const { return AO_Pitch; };
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; };
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE ULagCompensationComponent* GetLagCompensationComponent() const { return LagCompensationComponent; }
	FORCEINLINE UBuffComponent* GetBuffComponent() const { return BuffComponent; }
	FORCEINLINE TMap<FName, UBoxComponent*>& GetSsrCollisionBoxes() { return SsrCollisionBoxes; }

	FORCEINLINE void SetHealth(const float InHealth) { Health = InHealth; }
	FORCEINLINE void SetShield(const float InShield) { Shield = InShield; }

	AWeaponBase* GetEquippedWeapon() const;
	UStaticMeshComponent* GetGrenadeMesh() const;
	AShooterGameMode* GetShooterGameMode();
	AShooterPlayerState* GetShooterPlayerState();
	FVector GetHitTarget() const;
};
