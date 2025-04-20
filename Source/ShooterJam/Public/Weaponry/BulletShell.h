// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletShell.generated.h"

UCLASS()
class SHOOTERJAM_API ABulletShell : public AActor
{
	GENERATED_BODY()
	
public:	
	ABulletShell();

protected:
	virtual void BeginPlay() override;

	virtual void LaunchShell();	
	virtual void AddOnHit();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BulletShellMesh;

	UPROPERTY(EditAnywhere)
	float ShellEjectionImpulse{ 5.f };

	UPROPERTY(EditAnywhere)
	float ShellLifetime{ 30.f };

	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;
};
