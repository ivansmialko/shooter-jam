// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Animations/TurningInPlace.h"
#include "Interfaces/CrosshairsInteractable.h"

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

	float AO_Yaw;
	float AO_Pitch;
	float Root_AO_Yaw;
	FRotator StartingAimRotation;
	ETurningInPlace TurningInPlace;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeaponBase* LastOverlappedWeapon); //LastOverlappedWeapon is the last value of replicated variable, before it will be set

	UFUNCTION(Server, Reliable)
	void Server_OnEquip();
	UFUNCTION(Server, Reliable)
	void Server_OnAimStart();
	UFUNCTION(Server, Reliable)
	void Server_OnAimEnd();
	UFUNCTION(Server, Reliable)
	void Server_OnFireStart(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(Server, Reliable)
	void Server_OnFireEnd();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnFireStart(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnFireEnd();
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_OnHit();

	void ActionEquip();
	void ActionAimStart();
	void ActionAimEnd();
	void ActionFireStart(const FVector_NetQuantize& TraceHitTarget);
	void ActionFireEnd();

protected:
	virtual void BeginPlay() override;

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

	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnJump(const FInputActionValue& Value);
	void OnEquip(const FInputActionValue& Value);
	void OnCrouch(const FInputActionValue& Value);
	void OnAimStart(const FInputActionValue& Value);
	void OnAimEnd(const FInputActionValue& Value);
	void OnFireStart(const FInputActionValue& Value);
	void OnFireEnd(const FInputActionValue& Value);

	void CalculateAimOffset(float DeltaTime);
	void CalculateTurningInPlace(float DeltaTime);

	void CheckHidePlayerIfCameraClose();
	void PlayHitReactMontage();
public:
	AShooterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void Jump() override;

	void SetOverlappingWeapon(AWeaponBase* Weapon);
	bool GetIsWeaponEquipped();
	bool GetIsCrouched();
	bool GetIsAiming();
	float GetAoYaw();
	float GetAoPitch();
	ETurningInPlace GetTurningInPlace() const;
	AWeaponBase* GetEquippedWeapon() const;
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; };

	void PlayFireMontage(bool bInIsAiming);
	void OnHit();
};
