// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/ShooterCharacterAnimInstance.h"
#include "Characters/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weaponry/WeaponBase.h"

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
	bIsCrouched = ShooterCharacter->GetIsCrouched();
	bIsAiming = ShooterCharacter->GetIsAiming();
	bIsLocallyController = ShooterCharacter->IsLocallyControlled();
	EquippedWeapon = ShooterCharacter->GetEquippedWeapon();
	TurningInPlace = ShooterCharacter->GetTurningInPlace();
	bRotateRootBone = ShooterCharacter->GetRotateRootBone();
	bIsEliminated = ShooterCharacter->GetIsEliminated();
	bUseFabrik = ShooterCharacter->GetUseFabrik();
	bUseAimOffsets = ShooterCharacter->GetUseAimOffsets();
	bTransformRightHand = ShooterCharacter->GetUseRightHandTransform();
	bIsDancing = ShooterCharacter->GetIsDancing();
	
	//Offset yaw for strafing
	FRotator AimRotation = ShooterCharacter->GetBaseAimRotation(); 
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
	//This is to make smooth transition from -180 to 180
	FRotator DeltaRotationForFrame = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRotationForFrame, DeltaSeconds, 6.f);
	YawOffset = DeltaRotation.Yaw;

	//Calculate current camera rotation, to know how much character should "lean" while rotating
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();

	const FRotator RotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = RotationDelta.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f); //Make this leaning smooth, frame by frame
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	AO_Yaw = ShooterCharacter->GetAoYaw();
	AO_Pitch = ShooterCharacter->GetAoPitch();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && ShooterCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket", ERelativeTransformSpace::RTS_World));
		FVector OutPosition;
		FRotator OutRotation;
		ShooterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		//Rotate hand with weapon according to hit target
		if (ShooterCharacter->IsLocallyControlled())
		{
			FTransform RightHandTransform = ShooterCharacter->GetMesh()->GetSocketTransform(FName("hand_r", ERelativeTransformSpace::RTS_World));
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - ShooterCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaSeconds, 100.f);


			FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash", ERelativeTransformSpace::RTS_World));
			FVector MuzzleDirection = FRotationMatrix(MuzzleTipTransform.Rotator()).GetUnitAxis(EAxis::X);

			//Debug lines
			//DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleDirection * 1000.f, FColor::Red);
			//DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), ShooterCharacter->GetHitTarget(), FColor::Cyan);
		}
	}
}

void UShooterCharacterAnimInstance::Native_OnReloadFinished()
{
	if (!ShooterCharacter)
		return;

	ShooterCharacter->OnAnimReloadFinished();
}

void UShooterCharacterAnimInstance::Native_OnShellInserted()
{
	if (!ShooterCharacter)
		return;

	ShooterCharacter->OnAnimShellInserted();
}

void UShooterCharacterAnimInstance::Native_OnThrowFinished()
{
	if (!ShooterCharacter)
		return;

	ShooterCharacter->OnAnimThrowFinished();
}

void UShooterCharacterAnimInstance::Native_OnThrowLaunched()
{
	if (!ShooterCharacter)
		return;

	ShooterCharacter->OnAnimThrowLaunched();
}

void UShooterCharacterAnimInstance::Native_OnSwapSwapped()
{
	if (!ShooterCharacter)
		return;

	ShooterCharacter->OnAnimSwapSwapped();
}

void UShooterCharacterAnimInstance::Native_OnSwapFinished()
{
	if (!ShooterCharacter)
		return;

	ShooterCharacter->OnAnimSwapFinished();
}
