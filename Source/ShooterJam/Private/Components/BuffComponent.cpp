// Made by smialko


#include "Components/BuffComponent.h"
#include "Characters/ShooterCharacter.h"

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

	HealingTarget -= HealThisFrame;

	if (HealingTarget <= 0.f
		|| Character->GetHealth() >= Character->GetMaxHealth())
	{
		bIsHealing = false;
		HealingTarget = 0.f;
	}
}

void UBuffComponent::SetCharacter(AShooterCharacter* InCharacter)
{
	Character = InCharacter;
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateHealth(DeltaTime);
}

void UBuffComponent::AddHealth(float InHealth, float InHealTime /*= 0.f*/)
{
	bIsHealing = true;
	HealingRate = InHealth / InHealTime;

	HealingTarget += InHealth;
}
