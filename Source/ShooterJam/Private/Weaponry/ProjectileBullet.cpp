// Made by smialko


#include "Weaponry/ProjectileBullet.h"
#include "Characters/ShooterCharacter.h"
#include "PlayerControllers/ShooterCharacterController.h"
#include "Components/LagCompensationComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AShooterCharacter* OwnerCharacter = Cast<AShooterCharacter>(GetOwner());
	if (!OwnerCharacter)
		return;

	AShooterCharacterController* OwnerController = Cast<AShooterCharacterController>(OwnerCharacter->GetController());
	if (!OwnerController)
		return;

	if (HasAuthority() && !bUseServerSideRewind)
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
	}

	if (!HasAuthority() && bUseServerSideRewind)
	{
		AShooterCharacter* HitCharacter = Cast<AShooterCharacter>(OtherActor);
		if (OwnerCharacter && OwnerController && OwnerCharacter->GetLagCompensationComponent())
		{
			float HitTime{ OwnerController->GetServerTime() - OwnerController->GetSingleTripTime() };
			OwnerCharacter->GetLagCompensationComponent()->Server_ScoreRequestProjectile(HitCharacter, TraceStart, InitialVelocity, HitTime);
		}
	}

	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();

	FPredictProjectilePathParams PathParams;
	PathParams.bTraceWithChannel = true;
	PathParams.bTraceWithCollision = true;
	PathParams.DrawDebugTime = 5.f;
	PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	PathParams.LaunchVelocity = GetActorForwardVector() * InitialSpeed;
	PathParams.MaxSimTime = 4.f;
	PathParams.ProjectileRadius = 5.f;
	PathParams.SimFrequency = 30.f;
	PathParams.OverrideGravityZ = 0.3f;
	PathParams.StartLocation = GetActorLocation();
	PathParams.TraceChannel = ECollisionChannel::ECC_Visibility;
	PathParams.ActorsToIgnore.Add(this);
	FPredictProjectilePathResult PathResult;

	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);
}

#ifdef WITH_EDITOR
void AProjectileBullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (!ProjectileMovement)
		return;

	if (!PropertyChangedEvent.Property)
		return;

	FName PropertyName{ PropertyChangedEvent.Property->GetFName() };
	if (PropertyName != GET_MEMBER_NAME_CHECKED(AProjectileBullet, InitialSpeed))
		return;

	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = InitialSpeed;

}
#endif // WITH_EDITOR

AProjectileBullet::AProjectileBullet()
{
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->SetIsReplicated(true);
	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = InitialSpeed;

}
