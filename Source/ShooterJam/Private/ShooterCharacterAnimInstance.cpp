// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UShooterCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!ShooterCharacter)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	if(!ShooterCharacter)
		return;


	FVector CharacterVelocity{ ShooterCharacter->GetVelocity() };
	CharacterVelocity.Z = 0.f;
	Speed = CharacterVelocity.Size();
	
	UCharacterMovementComponent* CharacterMovement{ ShooterCharacter->GetCharacterMovement() };
	if(!CharacterMovement)
		return;

	bIsInAir = CharacterMovement->IsFalling();

	bIsAccelerating = (CharacterMovement->GetCurrentAcceleration().Size() > 0);

	bWeaponEquipped = ShooterCharacter->GetIsWeaponEquipped();
}
