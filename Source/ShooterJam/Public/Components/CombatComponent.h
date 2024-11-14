// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERJAM_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class AShooterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void EquipWeapon(class AWeaponBase* InWeaponToEquip);

private:
	AWeaponBase* EquippedWeapon;
	class AShooterCharacter* Character;

protected:
	virtual void BeginPlay() override;

public:
};
