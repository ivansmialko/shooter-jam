// Made by smialko


#include "Tutorials/WorldTutorialHint.h"

// Sets default values
AWorldTutorialHint::AWorldTutorialHint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWorldTutorialHint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWorldTutorialHint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

