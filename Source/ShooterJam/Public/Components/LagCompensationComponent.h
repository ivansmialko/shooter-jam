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

private:
	UPROPERTY()
	AShooterCharacter* Character;

	UPROPERTY()
	AShooterCharacterController* Controller;

public:	
	ULagCompensationComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	
	void SaveFramePackage(FFramePackage& InPack);

public:
	void SetCharacter(AShooterCharacter* InCharacter);
	void SetController(AShooterCharacterController* InController);
};
