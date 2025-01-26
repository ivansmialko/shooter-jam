#include "Weaponry/WeaponBase.h"
#include "Weaponry/BulletShell.h"
#include "Characters/ShooterCharacter.h"
#include "PlayerControllers/ShooterCharacterController.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"

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
		OnStateEquipped();
	} break;
	case EWeaponState::EWS_Dropped:
	{
		OnStateDropped();
	} break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}
}

void AWeaponBase::OnRep_Ammo()
{
	NotifyOwner_Ammo();
}

void AWeaponBase::OnStateEquipped()
{
	ShowPickUpWidget(false);
	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (!WeaponMesh)
		return;

	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponBase::OnStateDropped()
{
	if (!WeaponMesh)
		return;

	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	if (!HasAuthority())
		return;

	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeaponBase::SpawnBulletShell()
{
	if (!BulletShellClass)
		return;

	USkeletalMeshComponent* Mesh{ GetWeaponMesh() };
	if (!Mesh)
		return;

	const USkeletalMeshSocket* AmmoEjectSocket{ Mesh->GetSocketByName(FName("AmmoEject")) };
	if (!AmmoEjectSocket)
		return;

	UWorld* World{ GetWorld() };
	if (!World)
		return;

	FTransform EjectTransform{ AmmoEjectSocket->GetSocketTransform(Mesh) };

	World->SpawnActor<ABulletShell>(BulletShellClass, EjectTransform.GetLocation(), EjectTransform.GetRotation().Rotator());
}

void AWeaponBase::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	
	if (CheckInitOwner() && OwnerCharacter->HasAuthority())
	{
		NotifyOwner_Ammo();
	}
}

void AWeaponBase::NotifyOwner_Ammo()
{
	if (!CheckInitOwner())
		return;

	OwnerCharacter->OnSpendRound(this);
}

bool AWeaponBase::CheckInitOwner()
{
	if (OwnerCharacter && OwnerController)
		return true;

	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<AShooterCharacter>(GetOwner());
	}
	if (!OwnerCharacter)
		return false;

	if (!OwnerController)
	{
		OwnerController = Cast<AShooterCharacterController>(OwnerCharacter->GetController());
	}
	if (!OwnerCharacter)
		return false;

	return true;
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

	
	//Calculate fire delay	
	FireDelay = 60.f / static_cast<float>(FireRate);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("FireDelay, %f"), FireDelay));
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

void AWeaponBase::OnDropped()
{
	ChangeWeaponState(EWeaponState::EWS_Dropped);

	if (!WeaponMesh)
		return;

	WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	SetOwner(nullptr);
	OwnerCharacter = nullptr;
	OwnerController = nullptr;
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBase::OnRep_Owner()
{
	Super::OnRep_Owner();

	if (!GetOwner())
	{
		OwnerCharacter = nullptr;
		OwnerController = nullptr;
		return;
	}

	if (!CheckInitOwner())
		return;

	NotifyOwner_Ammo();
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponBase, WeaponState);
	DOREPLIFETIME(AWeaponBase, Ammo);
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
		OnStateEquipped();
	} break;
	case EWeaponState::EWS_Dropped:
	{
		OnStateDropped();
	} break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}

}

void AWeaponBase::Fire(const FVector& HitTarget)
{
	if (!FireAnimation)
		return;

	if (!WeaponMesh)
		return;

	WeaponMesh->PlayAnimation(FireAnimation, false);

	SpawnBulletShell();
	SpendRound();
}

