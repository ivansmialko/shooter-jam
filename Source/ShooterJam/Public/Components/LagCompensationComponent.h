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

	void ServerSideRewind(AShooterCharacter* InHitCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize& InHitLocation, float InHitTime);


//protected methods
protected:
	virtual void BeginPlay() override;
	
	void ShowFramePackage(const FFramePackage& InPackage, FColor InColor);
	void GetFramePackage(FFramePackage& InPack, AShooterCharacter* InCharacter = nullptr);
	void SaveFrame();
	void RewindPlayerBoxes(AShooterCharacter* InHitCharacter, const FFramePackage& InFramePackage, bool bInDisableCollision = false);
	void EnablePlayerCollisions(AShooterCharacter* InHitCharacter, ECollisionEnabled::Type InCollisionEnabled)

//private methods
private:
	FFramePackage InterpolateBetweenFrames(const FFramePackage& InOlderFrame, const FFramePackage& InYoungerFrame, float InHitTime);
	FSsrResult ConfirmHit(const FFramePackage& InFramePackage, AShooterCharacter* InCharacter, const FVector_NetQuantize& InTraceStart, const FVector_NetQuantize& InHitLocation);


//public getters/setters
public:
	void SetCharacter(AShooterCharacter* InCharacter);
	void SetController(AShooterCharacterController* InController);

	FORCEINLINE const TDoubleLinkedList<FFramePackage>& GetFrameHistory() const { return FrameHistory; }
};
