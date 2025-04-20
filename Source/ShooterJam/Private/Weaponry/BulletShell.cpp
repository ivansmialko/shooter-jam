// Made by smialko


#include "Weaponry/BulletShell.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ABulletShell::ABulletShell()
{
	PrimaryActorTick.bCanEverTick = false;

	BulletShellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletShellMesh"));
	BulletShellMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	BulletShellMesh->SetSimulatePhysics(true);
	BulletShellMesh->SetEnableGravity(true);
	BulletShellMesh->SetNotifyRigidBodyCollision(true);
	SetRootComponent(BulletShellMesh);

	ShellEjectionImpulse = 2.f;
}

void ABulletShell::BeginPlay()
{
	Super::BeginPlay();
	
	AddOnHit();
	LaunchShell();
	SetLifeSpan(ShellLifetime);
}

void ABulletShell::LaunchShell()
{
	if (!BulletShellMesh)
		return;

	BulletShellMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
}

void ABulletShell::AddOnHit()
{
	if (!BulletShellMesh)
		return;

	BulletShellMesh->OnComponentHit.AddDynamic(this, &ABulletShell::OnHit);
}

void ABulletShell::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}

	BulletShellMesh->SetNotifyRigidBodyCollision(false);
}

