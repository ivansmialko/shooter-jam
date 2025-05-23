// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector BoxExtent;
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time{ 0.f };

	TMap<FName, FBoxInformation> HitBoxInfo;
};

USTRUCT(BlueprintType)
struct FSsrResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed{ false };
	UPROPERTY()
	bool bHeadshot{ false };
};


class AShooterCharacter;
class AShooterCharacterController;
class AWeaponBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERJAM_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

//private fields
private:
	UPROPERTY()
	AShooterCharacter* Character;

	UPROPERTY()
	AShooterCharacterController* Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere)
	float MaxRecordTime{ 4.f };

//public methods
public:
	ULagCompensationComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Server, Reliable)
	void Server_ScoreRequest(AShooterCharacter* InHitCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize& InHitLocation, float InHitTime);

	UFUNCTION(Server, Reliable)
	void Server_ScoreRequestProjectile(AShooterCharacter* InHitCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize100& InitialVelocity, float InHitTime);
//protected methods
protected:
	virtual void BeginPlay() override;
	
	FSsrResult ServerSideRewind(AShooterCharacter* InHitCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize& InHitLocation, float InHitTime);
	FSsrResult ServerSideRewindProjectile(AShooterCharacter* InHitCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize100& InInitialVelocity, float InHitTime);
	FSsrResult ConfirmHit(const FFramePackage& InFramePackage, AShooterCharacter* InCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize& InHitLocation);
	FSsrResult ConfirmHitProjectile(const FFramePackage& InFramePackage, AShooterCharacter* InHitCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize100& InInitialVelocity, float InHitTime);


//private methods
private:
	FFramePackage InterpolateBetweenFrames(const FFramePackage& InOlderFrame, const FFramePackage& InYoungerFrame, float InHitTime);
	FFramePackage GetFrameToCheck(AShooterCharacter* InHitCharacter, float InHitTime);
	void ShowFramePackage(const FFramePackage& InPackage, FColor InColor);
	void GetFramePackage(FFramePackage& InPack, AShooterCharacter* InCharacter = nullptr);
	void SaveFrame();
	void ResetPlayerBoxes(AShooterCharacter* InHitCharacter, const FFramePackage& InFramePackage, bool bInDisableCollision = false);
	void EnablePlayerCollisions(AShooterCharacter* InHitCharacter, ECollisionEnabled::Type InCollisionEnabled);
	void EnableHitBoxesCollisions(AShooterCharacter* InHitCharacter, ECollisionEnabled::Type InCollisionEnabled, ECollisionResponse InCollisionResponse);

//public getters/setters
public:
	void SetCharacter(AShooterCharacter* InCharacter);
	void SetController(AShooterCharacterController* InController);

	FORCEINLINE const TDoubleLinkedList<FFramePackage>& GetFrameHistory() const { return FrameHistory; }
};
