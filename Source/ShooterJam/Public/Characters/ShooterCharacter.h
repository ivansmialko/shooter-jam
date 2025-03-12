// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Animations/TurningInPlace.h"
#include "Interfaces/CrosshairsInteractable.h"
#include "Components/CombatComponent.h"

#include "Components/TimelineComponent.h"

#include "ShooterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class UAnimMontage;
class USoundCue;
class UInputMappingContext;
class UInputAction;

class AShooterCharacterController;
class AShooterPlayerState;
class AShooterGameMode;
class AWeaponBase;

UCLASS()
class SHOOTERJAM_API AShooterCharacter : public ACharacter, public ICrosshairsInteractable
{
	GENERATED_BODY()

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

	/** Float curve that represents dissolving of character's mesh after dying */
	UPROPERTY(EditAnywhere, Category = Effects)
	UCurveFloat* DissolveCurve;

	/** Material instance of dissolve material. */
	UPROPERTY(EditAnywhere, Category = Effects)
	UMaterialInstance* DissolveMaterialInstance;				//Selected in the editor
	UPROPERTY(EditAnywhere, Category = Effects)

	/** Elimination bot particle system. */
	UParticleSystem* ElimbotEffect;								//Selected in the editor

	/** Sound of elimination. */
	UPROPERTY(EditAnywhere, Category = Effects)
	USoundCue* ElimbotSound;

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

	/** Current value of player's health */
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health{ MaxHealth };

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

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere)
	UCombatComponent* CombatComponent;
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	UPROPERTY(VisibleAnywhere, Category = Effects)
	UMaterialInstanceDynamic* DissolveMaterialInstanceDynamic;	//Instance of UMaterialInstance that we create in runtime
	UPROPERTY(VisibleAnywhere, Category = Effects)
	UParticleSystemComponent* ElimbotComponent;					//Instance of Particle system that we spawn in runtime
//~ End Exposed members

//~ Begin Replicated members
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeaponBase* OverlappingWeapon;
//~ End Replicated members

	bool bRotateRootBone;
	bool bInputInitialized{ false };
	float AO_Yaw;
	float AO_Pitch;
	float Root_AO_Yaw;
	float ProxyTurnTreshold{ 0.5f };
	float ProxyYaw;
	float TimeSinceLastMovementRep{ 0.f };
	FRotator ProxyRotation;
	FRotator ProxyRotationLastFrame;
	FRotator StartingAimRotation;
	
	ETurningInPlace TurningInPlace;
	
	FTimerHandle EliminatedTimer;
	FOnTimelineFloat DissolveTrackDlg;

	AShooterCharacterController* PlayerController{ nullptr };
	AShooterPlayerState* PlayerState{ nullptr };

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
	void OnRep_Health();
	virtual void OnRep_ReplicatedMovement() override;

	UFUNCTION()
	void OnReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageTypem, class AController* InstigatorController, AActor* DamageCauser);
	
	void OnEliminatedTimerFinished();
	UFUNCTION()
	void TimelineUpdateDissolveMaterial(float InDissolveValue);

	UFUNCTION(Server, Reliable)
	void Server_OnEquip();
	UFUNCTION(Server, Reliable)
	void Server_OnAimStart();
	UFUNCTION(Server, Reliable)
	void Server_OnAimEnd();
	UFUNCTION(Server, Reliable)
	void Server_OnReload();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnEliminated();

	void ActionEquip();
	void ActionAimStart();
	void ActionAimEnd();
	void ActionReceiveDamage();
	void ActionReload();
	void ActionThrow();

	void DropWeapon();

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

	void CalculateAimOffset(float DeltaTime);
	void CalculateAimOffset_SimProxies();
	void CalculateAimPitch();
	void CalculateTurningInPlace(float DeltaTime);
	float CalculateSpeed() const;

	void PollInit();
	void HudUpdateHealth();

	void CheckHidePlayerIfCameraClose();
	void PlayHitReactMontage();
	void PlayDissolvingEffect();
	void PlayElimbotEffect();
	void PlayReloadMontage(bool bInPlayReloadEnd);

	AShooterGameMode* GetShooterGameMode() const;

//public methods
public:
	AShooterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void Jump() override;
	virtual void Destroyed() override;

	virtual void OnEliminated();
	virtual void OnSpendRound(AWeaponBase* InWeapon);
	virtual void OnReloadFinished();
	virtual void OnShellInserted();
	virtual void OnThrowFinished();

	void PlayFireMontage(bool bInIsAiming);
	void PlayEliminationMontage();
	void PlayReloadMontage();
	void PlayReloadEndMontage();
	void PlayThrowMontage();

	void InitInputs();
	void DisableInputs();
	void DisableGameplay();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bInShowScope);

	//public getters/setters
public:

	void SetOverlappingWeapon(AWeaponBase* Weapon);

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
	FORCEINLINE float GetAoYaw() const { return AO_Yaw; };
	FORCEINLINE float GetAoPitch() const { return AO_Pitch; };
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; };

	AWeaponBase* GetEquippedWeapon() const;
	FVector GetHitTarget() const;
};
