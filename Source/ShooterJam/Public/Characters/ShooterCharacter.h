// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Animations/TurningInPlace.h"
#include "Interfaces/CrosshairsInteractable.h"

#include "Components/TimelineComponent.h"

#include "ShooterCharacter.generated.h"

UCLASS()
class SHOOTERJAM_API AShooterCharacter : public ACharacter, public ICrosshairsInteractable
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeaponBase* OverlappingWeapon;
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* CombatComponent;	
	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* FireWeaponMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimMontage* EliminationMontage;
	UPROPERTY(EditAnywhere, Category = Effects)
	UCurveFloat* DissolveCurve;
	UPROPERTY(EditAnywhere, Category = Effects)
	UMaterialInstance* DissolveMaterialInstance;				//Selected in the editor
	UPROPERTY(EditAnywhere, Category = Effects)
	UParticleSystem* ElimbotEffect;
	UPROPERTY(EditAnywhere, Category = Effects)
	class USoundCue* ElimbotSound;

	float AO_Yaw;
	float AO_Pitch;
	float Root_AO_Yaw;
	FRotator StartingAimRotation;
	ETurningInPlace TurningInPlace;

	bool bRotateRootBone;
	float ProxyTurnTreshold{ 0.5f };
	float ProxyYaw;
	float TimeSinceLastMovementRep{ 0.f };
	FRotator ProxyRotation;
	FRotator ProxyRotationLastFrame;

	FTimerHandle EliminatedTimer;
	UPROPERTY(EditDefaultsOnly)
	float EliminationDelay{ 3.f };

	class AShooterCharacterController* CharacterController{ nullptr };
	class AShooterPlayerState* PlayerState{ nullptr };

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeaponBase* LastOverlappedWeapon); //LastOverlappedWeapon is the last value of replicated variable, before it will be set
	UFUNCTION()
	void OnRep_Health();
	virtual void OnRep_ReplicatedMovement() override;

	UFUNCTION(Server, Reliable)
	void Server_OnEquip();
	UFUNCTION(Server, Reliable)
	void Server_OnAimStart();
	UFUNCTION(Server, Reliable)
	void Server_OnAimEnd();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnEliminated();

	void ActionEquip();
	void ActionAimStart();
	void ActionAimEnd();
	void ActionReceiveDamage();

	void DisableCharacter();
	void DropWeapon();

protected:
	//User input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* EquipAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, Category = Movement)
	float BaseWalkSpeed{ 600.f };
	UPROPERTY(EditAnywhere, Category = Movement)
	float AimWalkSpeed{ 450.f };
	UPROPERTY(EditAnywhere)
	double CameraDistanceTreshold{ 200.0 };

	//Player health fields
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth{ 100.f };
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health{ MaxHealth };
	UPROPERTY(VisibleAnywhere)
	bool bIsEliminated{ false };

	//Dissolve effect
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTrackDlg;
	UPROPERTY(VisibleAnywhere, Category = Effects)
	UMaterialInstanceDynamic* DissolveMaterialInstanceDynamic;	//Instance of UMaterialInstance that we create in runtime
	
	//Elimbot
	UPROPERTY(VisibleAnywhere, Category = Effects)
	UParticleSystemComponent* ElimbotComponent;

	virtual void BeginPlay() override;

	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnJump(const FInputActionValue& Value);
	void OnEquip(const FInputActionValue& Value);
	void OnCrouch(const FInputActionValue& Value);
	void OnAimStart(const FInputActionValue& Value);
	void OnAimEnd(const FInputActionValue& Value);
	void OnFireStart(const FInputActionValue& Value);
	void OnFireEnd(const FInputActionValue& Value);

	UFUNCTION()
	void OnReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageTypem, class AController* InstigatorController, AActor* DamageCauser);

	void OnEliminatedTimerFinished();

	void CalculateAimOffset(float DeltaTime);
	void CalculateAimOffset_SimProxies();
	void CalculateAimPitch();
	void CalculateTurningInPlace(float DeltaTime);
	float CalculateSpeed() const;

	void PollInit();

	void CheckHidePlayerIfCameraClose();
	void PlayHitReactMontage();
	void PlayDissolvingEffect();
	void PlayElimbotEffect();

	void HudUpdateHealth();

	UFUNCTION()
	void TimelineUpdateDissolveMaterial(float InDissolveValue);

	class AShooterGameMode* GetShooterGameMode() const;
public:
	AShooterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void Jump() override;
	virtual void OnEliminated();
	virtual void Destroyed() override;

	void SetOverlappingWeapon(AWeaponBase* Weapon);

	bool GetIsWeaponEquipped() const;
	bool GetIsCrouched() const;
	bool GetIsAiming() const;
	bool GetRotateRootBone() const;
	bool GetIsEliminated() const;
	float GetAoYaw() const;
	float GetAoPitch() const;
	float GetHealth() const;
	float GetMaxHealth() const;

	ETurningInPlace GetTurningInPlace() const;
	AWeaponBase* GetEquippedWeapon() const;
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; };

	void PlayFireMontage(bool bInIsAiming);
	void PlayEliminationMontage();
};
