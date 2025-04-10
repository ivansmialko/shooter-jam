#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimationAsset.h"

#include "Weaponry/WeaponTypes.h"

#include "WeaponBase.generated.h"


class USphereComponent;
class UWidgetComponent;
class UTexture2D;
class USoundCue;

class ABulletShell;
class AShooterCharacter;
class AShooterCharacterController;

UENUM()
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial state"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class SHOOTERJAM_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
//private members
private:

//~ Begin exposed members
	UPROPERTY(VisibleAnywhere, Category = "Weapon Physics")
	USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bIsAutomatic{ false };

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bIsReloadInterruptable{ false };

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	int32 FireRate{ 30 };

	UPROPERTY(EditAnywhere, Category = "Weapon Properties", ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	int32 MagCapacity;

	UPROPERTY(EditAnywhere, Category = "Weapon Style")
	TSubclassOf<ABulletShell> BulletShellClass;

	UPROPERTY(EditAnywhere, Category = "Weapon Style")
	UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = "Weapon Style")
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = "Weapon Style")
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = "Weapon Style")
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = "Weapon Style")
	UTexture2D* CrosshairsBottom;

	UPROPERTY(EditAnywhere, Category = "Weapon Style")
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, Category = "Weapon Feel")
	float FovZoomed{ 30.f };

	UPROPERTY(EditAnywhere, Category = "Weapon Feel")
	float ZoomInterpSpeed{ 20.f };

	UPROPERTY(EditAnywhere, Category = "Weapon Style")
	UAnimationAsset* FireAnimation;

	UPROPERTY(VisibleAnywhere, Category = "Weapon State", ReplicatedUsing = OnRep_WeaponState)
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = "Weapon State")
	UWidgetComponent* PickUpWidget;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* WeaponMesh;

//~ End exposed members

	UPROPERTY()
	AShooterCharacter* OwnerCharacter;

	UPROPERTY()
	AShooterCharacterController* OwnerController;
	
	//Calculated at beginplay, based on FireRate (bullets per minute)
	float FireDelay{ 0 };

//protected members
protected:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float BaseDamage{ 20.f };

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bUseScatter{ false };

	UPROPERTY(EditAnywhere, Category = "Weapon Properties", meta = (EditCondition = "bUseScatter"))
	float ScatterSphereDistance{ 800.f };

	UPROPERTY(EditAnywhere, Category = "Weapon Properties", meta = (EditCondition = "bUseScatter"))
	float ScatterSphereRadius{ 75.f };

//private methods
private:
	UFUNCTION()
	void OnRep_WeaponState();
	UFUNCTION()
	void OnRep_Ammo();

	void OnStateEquipped();
	void OnStateDropped();

	void SpawnBulletShell();
	void SpendRound();

	bool CheckInitOwner();

//public methods
public:
	AWeaponBase();

	//~ Begin AActor Inteface
	virtual void Tick(float DeltaTime) override;
	virtual void OnRep_Owner() override;
	//~ End AActor Interface

	//~ Begin UObject Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface

	void ShowPickUpWidget(bool bShowWidget);
	void ChangeWeaponState(EWeaponState InState);
	void AddAmmo(int32 AmmoToAdd);
	void NotifyOwner_Ammo();
	void EnableCustomDepth(bool bInEnable);

	virtual void Fire(const FVector& HitTarget);

	void OnDropped();

//protected methods
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnAreaSphereOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnAreaSphereOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex);

	FVector GetTraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	FVector GetTraceEnd(const FVector& TraceStart, const FVector& HitTarget);
	void HitScan(FHitResult& OutHitResult, const FVector& TraceStart, const FVector& TraceEnd);

//public getters
public:
	FORCEINLINE bool IsEmpty() const { return Ammo <= 0; }
	FORCEINLINE bool IsFull() const { return GetWeaponAmmo() == GetMagCapacity(); }
	FORCEINLINE bool GetIsAutomatic() const { return bIsAutomatic; }
	FORCEINLINE bool GetIsReloadInterruptable() const { return bIsReloadInterruptable; }
	FORCEINLINE int32 GetWeaponAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE float GetFovZoomed() const { return FovZoomed; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE float GetFireDelay() const { return FireDelay; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE UTexture2D* GetCrosshairsCenter() const { return CrosshairsCenter; }
	FORCEINLINE UTexture2D* GetCrosshairsLeft() const { return CrosshairsLeft; }
	FORCEINLINE UTexture2D* GetCrosshairsRight() const { return CrosshairsRight; }
	FORCEINLINE UTexture2D* GetCrosshairsTop() const { return CrosshairsTop; }
	FORCEINLINE UTexture2D* GetCrosshairsBottom() const { return CrosshairsBottom; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
	FTransform GetMuzzleTransform() const;
};
