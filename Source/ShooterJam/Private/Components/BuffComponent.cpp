// Made by smialko


#include "Components/BuffComponent.h"
#include "Characters/ShooterCharacter.h"

void UBuffComponent::OnSpeedBuffTimerFinished()
{
	Character->SetWalkSpeed(InitialBaseSpeed);
	Character->SetCrouchSpeed(InitialCrouchSpeed);

	Multicast_AddSpeed(InitialBaseSpeed, InitialCrouchSpeed);
}

void UBuffComponent::OnJumpBuffTimerFinished()
{

}

void UBuffComponent::Multicast_AddSpeed_Implementation(float InBaseSpeed, float InCrouchSpeed)
{
	if (!Character)
		return;

	Character->SetWalkSpeed(InBaseSpeed);
	Character->SetCrouchSpeed(InCrouchSpeed);
}

void UBuffComponent::Multicast_AddJump_Implementation(float InJumpVelocityBoost)
{
	if (!Character)
		return;

	Character->SetWalkSpeed(InJumpVelocityBoost);
}

UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UBuffComponent::UpdateHealth(float InDeltaTime)
{
	if (!bIsHealing)
		return;

	if (!Character)
		return;

	if (Character->GetIsEliminated())
		return;	

	const float HealThisFrame = HealingRate * InDeltaTime;
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0, Character->GetMaxHealth()));
	Character->HudUpdateHealth();

	HealingTarget -= HealThisFrame;

	if (HealingTarget <= 0.f
		|| Character->GetHealth() >= Character->GetMaxHealth())
	{
		bIsHealing = false;
		HealingTarget = 0.f;
	}
}

void UBuffComponent::UpdateShield(float InDeltaTime)
{
	if (!bIsShielding)
		return;

	if (!Character)
		return;

	if (Character->GetIsEliminated())
		return;

	const float ShieldThisFrame = HealingRate * InDeltaTime;
	Character->SetShield(FMath::Clamp(Character->GetShield() + ShieldThisFrame, 0, Character->GetMaxShield()));
	Character->HudUpdateShield();

	ShieldingTarget -= ShieldThisFrame;

	if (ShieldingTarget <= 0.f
		|| Character->GetShield() >= Character->GetMaxShield())
	{
		bIsShielding = false;
		ShieldingTarget = 0.f;
	}
}

void UBuffComponent::SetCharacter(AShooterCharacter* InCharacter)
{
	Character = InCharacter;
}

void UBuffComponent::SetInitialBaseSpeed(float InBaseSpeed)
{
	InitialBaseSpeed = InBaseSpeed;
}

void UBuffComponent::SetInitialCrouchSpeed(float InBaseCrouchSpeed)
{
	InitialCrouchSpeed = InBaseCrouchSpeed;
}

void UBuffComponent::SetInitialJumpVelocity(float InBaseJumpVelocity)
{
	InitialJumpVelocity = InBaseJumpVelocity;
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateHealth(DeltaTime);
	UpdateShield(DeltaTime);
}

void UBuffComponent::AddHealth(float InHealth, float InHealTime /*= 0.f*/)
{
	bIsHealing = true;
	HealingRate = InHealth / InHealTime;

	HealingTarget += InHealth;
}

void UBuffComponent::AddSpeed(float InWalkSpeed, float InCrouchSpeed, float InDuration)
{
	if (!Character)
		return;

	Character->GetWorldTimerManager().SetTimer(SpeedBuffTimer, this, &UBuffComponent::OnSpeedBuffTimerFinished, InDuration);
	Character->SetWalkSpeed(InWalkSpeed);
	Character->SetCrouchSpeed(InCrouchSpeed);
	Multicast_AddSpeed(InWalkSpeed, InCrouchSpeed);
}

void UBuffComponent::AddJump(float InJumpVelocityBoost, float InDuration)
{
	if (!Character)
		return;

	Character->GetWorldTimerManager().SetTimer(JumpBuffTimer, this, &UBuffComponent::OnSpeedBuffTimerFinished, InDuration);
	Character->SetJumpVelocity(InJumpVelocityBoost);
	Multicast_AddJump_Implementation(InJumpVelocityBoost);
	
}

void UBuffComponent::AddShield(float InShield, float InShieldTime /*= 0.f*/)
{
	bIsShielding = true;
	ShieldingRate = InShield / InShieldTime;

	ShieldingTarget += InShield;
}
