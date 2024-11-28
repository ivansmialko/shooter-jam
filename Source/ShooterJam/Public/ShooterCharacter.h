// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class SHOOTERJAM_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	void SetOverlappingWeapon(AWeaponBase* Weapon);
	bool GetIsWeaponEquipped();
	bool GetIsCrouched();
	bool GetIsAiming();
	float GetAoYaw();
	float GetAoPitch();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* AimAction;

	UPROPERTY(EditAnywhere, Category = Movement)
	float BaseWalkSpeed{ 600.f };
	UPROPERTY(EditAnywhere, Category = Movement)
	float AimWalkSpeed{ 450.f };

	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnJump(const FInputActionValue& Value);
	void OnEquip(const FInputActionValue& Value);
	void OnCrouch(const FInputActionValue& Value);
	void OnAimStart(const FInputActionValue& Value);
	void OnAimEnd(const FInputActionValue& Value);

	void CalculateAimOffset(float DeltaTime);

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

	float AO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeaponBase* LastOverlappedWeapon); //LastOverlappedWeapon is the last value of replicated variable, before it will be set

	UFUNCTION(Server, Reliable)
	void Server_OnEquip();
	UFUNCTION(Server, Reliable)
	void Server_OnAimStart();
	UFUNCTION(Server, Reliable)
	void Server_OnAimEnd();

	void ActionEquip();
	void ActionAimStart();
	void ActionAimEnd();
};
