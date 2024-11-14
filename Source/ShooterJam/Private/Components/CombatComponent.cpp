// Made by smialko

#include "ShooterCharacter.h"
#include "Components/CombatComponent.h"
#include "Weaponry/WeaponBase.h"
#include "Engine/SkeletalMeshSocket.h"

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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString("Has character"));


	if (!InWeaponToEquip)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString("Has weapon to equip"));

	EquippedWeapon = InWeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	const USkeletalMeshSocket* HandSocket{ Character->GetMesh()->GetSocketByName(FName("RightHandSocket")) };
	if (!HandSocket)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString("Has socket"));

	HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());

	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->ShowPickUpWidget(false);
}

