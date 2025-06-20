#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimationAsset.h"

#include "Weaponry/WeaponTypes.h"

#include "WeaponBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponPickedUp);

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
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),
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
	bool bIsDestroyAfterDeath{ false };

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	int32 FireRate{ 30 };

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
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
	
	//Calculated at beginplay, based on FireRate (bullets per minute)
	float FireDelay{ 0 };

//public members
public:
	FOnWeaponPickedUp OnWeaponPickedUp;

//protected members
protected:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float BaseDamage{ 20.f };

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float HeadDamage{ 40.f };

	/** Enables server-side rewind for the weapon */
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bUseServerSideRewind{ false };

	/** Tells if weapon currently using ssr. It can be different from design value (upper one) in situations like too high ping */
	UPROPERTY(Replicated)
	bool bCurrentlyUsingSsr{ false };

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bUseScatter{ false };

	UPROPERTY(EditAnywhere, Category = "Weapon Properties", meta = (EditCondition = "bUseScatter"))
	uint32 ScatterHitsNumber{ 10 };

	UPROPERTY(EditAnywhere, Category = "Weapon Properties", meta = (EditCondition = "bUseScatter"))
	float ScatterSphereDistance{ 800.f };

	UPROPERTY(EditAnywhere, Category = "Weapon Properties", meta = (EditCondition = "bUseScatter"))
	float ScatterSphereRadius{ 75.f };

	/** Array of points in world, weapons should fire into */
	TArray<FVector> HitTargets;

	/** The number of unprocessed server requests for Ammo. Incremented in SpendRound, decremented in Client_UpdateAmmo */
	int32 Sequence{ 0 };

	UPROPERTY()
	AShooterCharacter* OwnerCharacter;

	UPROPERTY()
	AShooterCharacterController* OwnerController;
	
//private methods
private:
	UFUNCTION()
	void OnRep_WeaponState();

	UFUNCTION(Client, Reliable)
	void Client_UpdateAmmo(int32 ServerAmmo);

	UFUNCTION(Client, Reliable)
	void Client_AddAmmo(int32 AmmoToAdd);

	void OnStateEquipped();
	void OnStateDropped();
	void OnStateEquippedSecondary();

	void SpawnBulletShell();
	void SpendRound();

	void PlayFireAnimation();

	bool CheckInitOwner();

	void ClearHitTargets();

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

	/**
	 * Base method to fire weapon. Plays animation and spends bullets. Child classes are responsible for projectile/hit-scan spawning, etc.
	 */
	virtual void Fire();

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

	FVector GetTraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget) const;
	FVector GetTraceEnd(const FVector& TraceStart, const FVector& HitTarget) const;
	void HitScan(FHitResult& OutHitResult, const FVector& TraceStart, const FVector& TraceEnd);

	virtual void DealDamage(const FHitResult& HitResult, const FVector_NetQuantize& TraceStart);

//public getters/setters
public:
	FORCEINLINE bool IsEmpty() const { return Ammo <= 0; }
	FORCEINLINE bool IsFull() const { return GetWeaponAmmo() == GetMagCapacity(); }
	FORCEINLINE bool GetIsAutomatic() const { return bIsAutomatic; }
	FORCEINLINE bool GetIsReloadInterruptable() const { return bIsReloadInterruptable; }
	FORCEINLINE bool GetIsDestroyAfterDeath() const { return bIsDestroyAfterDeath; }
	FORCEINLINE bool GetIsUsingScatter() const { return bUseScatter; }
	FORCEINLINE bool GetIsUsingSsr() const { return bUseServerSideRewind; }
	FORCEINLINE bool GetIsCurrentlyUsingSsr() const { return bCurrentlyUsingSsr; }
	FORCEINLINE int32 GetWeaponAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE int32 GetScatterHitsNumber() const { return ScatterHitsNumber; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE float GetFovZoomed() const { return FovZoomed; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE float GetFireDelay() const { return FireDelay; }
	FORCEINLINE float GetBaseDamage() const { return BaseDamage; }
	FORCEINLINE float GetHeadDamage() const { return HeadDamage; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE UTexture2D* GetCrosshairsCenter() const { return CrosshairsCenter; }
	FORCEINLINE UTexture2D* GetCrosshairsLeft() const { return CrosshairsLeft; }
	FORCEINLINE UTexture2D* GetCrosshairsRight() const { return CrosshairsRight; }
	FORCEINLINE UTexture2D* GetCrosshairsTop() const { return CrosshairsTop; }
	FORCEINLINE UTexture2D* GetCrosshairsBottom() const { return CrosshairsBottom; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
	FTransform GetMuzzleTransform() const;
	TArray<FVector> GetHitTargets(const FVector& InTraceStart, const FVector& InDirection) const;
	TArray<FVector_NetQuantize> GetHitTargetsNet(const FVector InTraceStart, const FVector InDirection) const;

	FORCEINLINE void SetIsDestroyAfterDeath(bool bInIsDestroy) { bIsDestroyAfterDeath = bInIsDestroy; }
	FORCEINLINE void AddHitTarget(const FVector& InHitTarget) { HitTargets.Add(InHitTarget); }
	FORCEINLINE void AddHitTarget(const FVector_NetQuantize& InHitTarget) { HitTargets.Add(InHitTarget); }
	void AddHitTarget(const TArray<FVector_NetQuantize>& InHitTarget);
	FORCEINLINE void SetUseSsr(bool bInUseSsr) { bUseServerSideRewind = bInUseSsr; }
	FORCEINLINE void SetCurrentlyUseSsr(bool bInCurrentlyUseSsr) { bCurrentlyUsingSsr = bInCurrentlyUseSsr; }
};
