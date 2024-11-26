// Made by smialko

#include "Components/CombatComponent.h"
#include "ShooterCharacter.h"
#include "Weaponry/WeaponBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
}

void UCombatComponent::EquipWeapon(class AWeaponBase* InWeaponToEquip)
{
	if (!Character)
		return;

	if (!InWeaponToEquip)
		return;

	EquippedWeapon = InWeaponToEquip;

	const USkeletalMeshSocket* HandSocket{ Character->GetMesh()->GetSocketByName(FName("RightHandSocket")) };
	if (!HandSocket)
		return;

	HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	EquippedWeapon->ChangeWeaponState(EWeaponState::EWS_Equipped);
	EquippedWeapon->SetOwner(Character);
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

bool UCombatComponent::GetIsWeaponEquipped()
{
	return (EquippedWeapon != nullptr);
}

bool UCombatComponent::GetIsAiming()
{
	return bIsAiming;
}

void UCombatComponent::SetIsAiming(bool bInIsAiming)
{
	bIsAiming = bInIsAiming;
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Received replication"));

	if (!EquippedWeapon)
		return;

	UE_LOG(LogTemp, Warning, TEXT("Weapon is ok"));

	if (!Character)
		return;

	UE_LOG(LogTemp, Warning, TEXT("Character is ok"));

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

