#include "Weaponry/WeaponBase.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "ShooterCharacter.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(WeaponMesh);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(RootComponent);
}

void AWeaponBase::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
	{
		ShowPickUpWidget(false);
	} break;
	case EWeaponState::EWS_Dropper:
		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnAreaSphereOverlapBegin);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::OnAreaSphereOverlapEnd);
	}

	if (PickUpWidget)
	{
		PickUpWidget->SetVisibility(false);
	}
}

void AWeaponBase::OnAreaSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (!ShooterCharacter)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString("Setting overlapping widget"));
	ShooterCharacter->SetOverlappingWeapon(this);
}

void AWeaponBase::OnAreaSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (!ShooterCharacter)
		return;

	ShooterCharacter->SetOverlappingWeapon(nullptr);
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponBase, WeaponState);
}

void AWeaponBase::ShowPickUpWidget(bool bShowWidget)
{
	if (!PickUpWidget)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString("Hiding widget"));
	PickUpWidget->SetVisibility(bShowWidget);
}

void AWeaponBase::ChangeWeaponState(EWeaponState InState)
{
	WeaponState = InState;

	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
	{
		ShowPickUpWidget(false);
		GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	} break;
	case EWeaponState::EWS_Dropper:
		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}

}


