// Made by smialko


#include "Tutorials/WorldTutorialTrigger.h"

// Sets default values
AWorldTutorialTrigger::AWorldTutorialTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWorldTutorialTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWorldTutorialTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

