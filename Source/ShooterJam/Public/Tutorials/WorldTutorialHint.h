// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldTutorialHint.generated.h"

class UWidgetComponent;

UCLASS()
class SHOOTERJAM_API AWorldTutorialHint : public AActor
{
	GENERATED_BODY()

//private
private:
	/** Blueprint widget to display in the world */
	UPROPERTY(EditAnywhere)
	UWidgetComponent* HintWidget{ nullptr };

public:	
	// Sets default values for this actor's properties
	AWorldTutorialHint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
