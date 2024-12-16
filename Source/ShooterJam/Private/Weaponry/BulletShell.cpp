// Made by smialko


#include "Weaponry/BulletShell.h"

ABulletShell::ABulletShell()
{
	PrimaryActorTick.bCanEverTick = false;

	BulletShellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletShellMesh"));
	SetRootComponent(BulletShellMesh);
}

void ABulletShell::BeginPlay()
{
	Super::BeginPlay();
	
}

