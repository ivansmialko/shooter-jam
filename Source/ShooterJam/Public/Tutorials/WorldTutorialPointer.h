// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldTutorialPointer.generated.h"

class UWidgetComponent;

UCLASS()
class SHOOTERJAM_API AWorldTutorialPointer : public AActor
{
	GENERATED_BODY()

//private fields
private:

//public methods
public:	
	// Sets default values for this actor's properties
	AWorldTutorialPointer();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

//protected methods
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
