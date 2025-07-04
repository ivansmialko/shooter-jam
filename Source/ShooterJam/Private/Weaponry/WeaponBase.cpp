#include "Weaponry/WeaponBase.h"
#include "Weaponry/BulletShell.h"
#include "Characters/ShooterCharacter.h"
#include "PlayerControllers/ShooterCharacterController.h"
#include "Components/LagCompensationComponent.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);
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
	if (OwnerCharacter)
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("[%s][%s] Weapon %s changed it's state to %d"),
			(OwnerCharacter->GetLocalRole() == ENetRole::ROLE_Authority ? TEXT("Server") : TEXT("Client")),
			(OwnerCharacter->IsLocallyControlled() ? TEXT("Local") : TEXT("Remote")),
			(*GetActorLabel()),
			static_cast<uint32>(WeaponState));
#endif
	}

	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
	{
		OnStateEquipped();
	} break;
	case EWeaponState::EWS_Dropped:
	{
		OnStateDropped();
	} break;
	case EWeaponState::EWS_EquippedSecondary:
	{
		OnStateEquippedSecondary();
	} break;
	default:
		break;
	}
}

void AWeaponBase::Client_UpdateAmmo_Implementation(int32 ServerAmmo)
{
	if (HasAuthority())
		return;

	Ammo = ServerAmmo;
	--Sequence;
	Ammo -= Sequence; //Client reconciliation

	NotifyOwner_Ammo();
}

void AWeaponBase::Client_AddAmmo_Implementation(int32 AmmoToAdd)
{
	if (HasAuthority())
		return;

	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);

	NotifyOwner_Ammo();
}

void AWeaponBase::OnStateEquipped()
{
	ShowPickUpWidget(false);
	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnableCustomDepth(false);

	if (!WeaponMesh)
		return;

	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (WeaponType == EWeaponType::EWT_SMG)
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}

	if (!HasAuthority())
		return;

	OnWeaponPickedUp.Broadcast();
}

void AWeaponBase::OnStateDropped()
{
	if (!WeaponMesh)
		return;

	EnableCustomDepth(true);

	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	if (!HasAuthority())
		return;

	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void AWeaponBase::OnStateEquippedSecondary()
{
	ShowPickUpWidget(false);
	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnableCustomDepth(false);

	if (!WeaponMesh)
		return;

	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (WeaponType == EWeaponType::EWT_SMG)
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}

	if (!HasAuthority())
		return;

	OnWeaponPickedUp.Broadcast();
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
	NotifyOwner_Ammo();

	if (HasAuthority())
	{
		Client_UpdateAmmo(Ammo);
	}
	else
	{
		++Sequence;
	}
}

void AWeaponBase::PlayFireAnimation()
{
	if (!FireAnimation)
		return;

	if (!WeaponMesh)
		return;

	WeaponMesh->PlayAnimation(FireAnimation, false);
}

void AWeaponBase::NotifyOwner_Ammo()
{
	if (!CheckInitOwner())
		return;

	OwnerCharacter->OnSpendRound(this);
}

void AWeaponBase::EnableCustomDepth(bool bInEnable)
{
	if (!WeaponMesh)
		return;

	WeaponMesh->SetRenderCustomDepth(bInEnable);
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

void AWeaponBase::ClearHitTargets()
{
	HitTargets.Empty();
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnAreaSphereOverlapBegin);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::OnAreaSphereOverlapEnd);

	if (PickUpWidget)
	{
		PickUpWidget->SetVisibility(false);
	}

	//Calculate fire delay	
	FireDelay = 60.f / static_cast<float>(FireRate);
	bCurrentlyUsingSsr = bUseServerSideRewind;
}

void AWeaponBase::OnAreaSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (!ShooterCharacter)
		return;

	ShooterCharacter->SetOverlappingWeapon(this);
}

void AWeaponBase::OnAreaSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (!ShooterCharacter)
		return;

	ShooterCharacter->SetOverlappingWeapon(nullptr);
}

FVector AWeaponBase::GetTraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget) const
{
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormalized * ScatterSphereDistance;

	FVector RandomPoint = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, ScatterSphereRadius);

	FVector EndLocation = SphereCenter + RandomPoint;
	FVector ToEndLocation = EndLocation - TraceStart;

	//DrawDebugSphere(GetWorld(), SphereCenter, ScatterSphereRadius, 12, FColor::Red, true);
	//DrawDebugSphere(GetWorld(), EndLocation, 4.f, 12.f, FColor::Cyan, true);
	//DrawDebugLine(GetWorld(), TraceStart, FVector(TraceStart + ToEndLocation * TRACE_LENGTH / ToEndLocation.Size()), FColor::Green, true);

	return FVector(TraceStart + ToEndLocation * TRACE_LENGTH / ToEndLocation.Size());
}

FVector AWeaponBase::GetTraceEnd(const FVector& TraceStart, const FVector& HitTarget) const
{
	return (TraceStart + (HitTarget - TraceStart) * 1.25);
}

void AWeaponBase::HitScan(FHitResult& OutHitResult, const FVector& TraceStart, const FVector& TraceEnd)
{

	UWorld* World = GetWorld();
	if (!World)
		return;

	World->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
}

void AWeaponBase::DealDamage(const FHitResult& HitResult, const FVector_NetQuantize& TraceStart)
{
	AShooterCharacter* HitCharacter = Cast<AShooterCharacter>(HitResult.GetActor());
	if (!HitCharacter)
		return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
		return;

	AController* InstigatorController = OwnerPawn->GetController();
	if (!InstigatorController)
		return;

	bool bIsHeadShot{ HitResult.BoneName == FString("head") };

	float CurrentDamage{ bIsHeadShot ? HeadDamage : BaseDamage };

	if (HasAuthority() && (!bCurrentlyUsingSsr || OwnerPawn->IsLocallyControlled()))
	{
		UGameplayStatics::ApplyDamage(HitCharacter, CurrentDamage, InstigatorController, this, UDamageType::StaticClass());
	}
	
	if(!HasAuthority() && bCurrentlyUsingSsr)
	{
		OwnerCharacter = (OwnerCharacter ? OwnerCharacter : Cast<AShooterCharacter>(OwnerPawn));
		OwnerController = (OwnerController ? OwnerController : Cast<AShooterCharacterController>(InstigatorController));

		if (OwnerCharacter && OwnerController && OwnerCharacter->GetLagCompensationComponent())
		{
			float HitTime{ OwnerController->GetServerTime() - OwnerController->GetSingleTripTime() };
			OwnerCharacter->GetLagCompensationComponent()->Server_ScoreRequest(HitCharacter, TraceStart, HitResult.ImpactPoint, HitTime);
		}
	}
}

FTransform AWeaponBase::GetMuzzleTransform() const
{
	if (!GetWeaponMesh())
		return FTransform();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (!MuzzleFlashSocket)
		return FTransform();

	return MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
}

TArray<FVector> AWeaponBase::GetHitTargets(const FVector& InTraceStart, const FVector& InDirection) const
{
	TArray<FVector> CurrHitTargets;

	if (bUseScatter)
	{
		for (uint32 i = 0; i < ScatterHitsNumber; ++i)
		{
			CurrHitTargets.Add(GetTraceEndWithScatter(InTraceStart, InDirection));
		}
	}
	else
	{
		CurrHitTargets.Add(GetTraceEnd(InTraceStart, InDirection));
	}

	return CurrHitTargets;
}

TArray<FVector_NetQuantize> AWeaponBase::GetHitTargetsNet(const FVector InTraceStart, const FVector InDirection) const
{
	TArray<FVector_NetQuantize> CurrHitTargets;

	if (bUseScatter)
	{
		for (uint32 i = 0; i < ScatterHitsNumber; ++i)
		{
			CurrHitTargets.Add(GetTraceEndWithScatter(InTraceStart, InDirection));
		}
	}
	else
	{
		CurrHitTargets.Add(GetTraceEnd(InTraceStart, InDirection));
	}

	return CurrHitTargets;
}

void AWeaponBase::AddHitTarget(const TArray<FVector_NetQuantize>& InHitTargets)
{
	for (int32 i = 0; i < InHitTargets.Num(); ++i)
	{
		HitTargets.Add(InHitTargets[i]);
	}
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
	DOREPLIFETIME_CONDITION(AWeaponBase, bCurrentlyUsingSsr, COND_OwnerOnly);
}

void AWeaponBase::ShowPickUpWidget(bool bShowWidget)
{
	if (!PickUpWidget)
		return;

	PickUpWidget->SetVisibility(bShowWidget);
}

void AWeaponBase::ChangeWeaponState(EWeaponState InState)
{
	WeaponState = InState;
	OnRep_WeaponState();
}

void AWeaponBase::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	Client_AddAmmo(AmmoToAdd);

	if (!CheckInitOwner())
		return;

	if (!OwnerCharacter->HasAuthority())
		return;

	//Only for server player. Clients will be updated through OnRep_Ammo
	NotifyOwner_Ammo();
}

void AWeaponBase::Fire()
{
	SpawnBulletShell();
	PlayFireAnimation();
	ClearHitTargets();

	SpendRound();
}

