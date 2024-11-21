// Made by smialko

#include "Components/CombatComponent.h"
#include "ShooterCharacter.h"
#include "Weaponry/WeaponBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "ShooterCharacter.h"

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
}

