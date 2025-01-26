#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimationAsset.h"
#include "WeaponBase.generated.h"

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
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickUpWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bIsAutomatic{ false };

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	int32 FireRate{ 30 };

	//Calculated at beginplay, based on FireRate (bullets per minute)
	float FireDelay{ 0 };

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABulletShell> BulletShellClass;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsBottom;

	UPROPERTY(EditAnywhere, Category = Zoom)
	float FovZoomed{ 30.f };

	UPROPERTY(EditAnywhere, Category = Zoom)
	float ZoomInterpSpeed{ 20.f };

	UPROPERTY(EditAnywhere, Category = Ammo, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;

	UPROPERTY(EditAnywhere, Category = Ammo)
	int32 MagCapacity;

	UPROPERTY()
	class AShooterCharacter* OwnerCharacter;
	UPROPERTY()
	class AShooterCharacterController* OwnerController;

//protected members
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
	void NotifyOwner_Ammo();

	virtual void Fire(const FVector& HitTarget);

	void OnDropped();

//public getters
public:
	FORCEINLINE class USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE UTexture2D* GetCrosshairsCenter() const { return CrosshairsCenter; }
	FORCEINLINE UTexture2D* GetCrosshairsLeft() const { return CrosshairsLeft; }
	FORCEINLINE UTexture2D* GetCrosshairsRight() const { return CrosshairsRight; }
	FORCEINLINE UTexture2D* GetCrosshairsTop() const { return CrosshairsTop; }
	FORCEINLINE UTexture2D* GetCrosshairsBottom() const { return CrosshairsBottom; }
	FORCEINLINE float GetFovZoomed() const { return FovZoomed; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE bool GetIsAutomatic() const { return bIsAutomatic; }
	FORCEINLINE float GetFireDelay() const { return FireDelay; }
	FORCEINLINE int32 GetWeaponAmmo() const { return Ammo; }
	FORCEINLINE bool IsEmpty() const { return Ammo <= 0; }
};
