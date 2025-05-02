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
	float Time;

	TMap<FName, FBoxInformation> HitBoxInfo;
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
	void GetFramePackage(FFramePackage& InPack);
	void SaveFrame();

//private methods
private:
	FFramePackage InterpolateBetweenFrames(const FFramePackage& InFirst, const FFramePackage& InSecond, float HitTime);


//public getters/setters
public:
	void SetCharacter(AShooterCharacter* InCharacter);
	void SetController(AShooterCharacterController* InController);

	FORCEINLINE const TDoubleLinkedList<FFramePackage>& GetFrameHistory() const { return FrameHistory; }
};
