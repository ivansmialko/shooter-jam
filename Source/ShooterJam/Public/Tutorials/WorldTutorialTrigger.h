// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldTutorialTrigger.generated.h"

UCLASS()
class SHOOTERJAM_API AWorldTutorialTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldTutorialTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
