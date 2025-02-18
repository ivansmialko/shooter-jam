// Made by smialko


#include "Weaponry/ProjectileRocket.h"

#include "Kismet/GameplayStatics.h"

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	APawn* FiringPawn = GetInstigator();
	if (!FiringPawn)
		return;

	AController* FiringController = Cast<AController>(FiringPawn->GetController());
	if (!FiringController)
		return;

	UGameplayStatics::ApplyRadialDamageWithFalloff(
		this,						//World context
		Damage,						//Base damage
		10.f,						//Minimal damage
		GetActorLocation(),			//Origin
		200.f,						//Inner radius
		500.f,						//Outer radius
		1.f,						//Falloff
		UDamageType::StaticClass(),	//Damage type
		TArray<AActor*>(),			//Actors to ignore
		this,						//Damage causer
		FiringController			//Instigator
	);

	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

AProjectileRocket::AProjectileRocket()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
