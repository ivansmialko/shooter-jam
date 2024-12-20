// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage 
{
	GENERATED_BODY()

public:
	class UTexture2D* CrosshairsCenter{ nullptr };
	UTexture2D* CrosshairsLeft{ nullptr };
	UTexture2D* CrosshairsRight{ nullptr };
	UTexture2D* CrosshairsTop{ nullptr };
	UTexture2D* CrosshairsBottom{ nullptr };
	float CrosshairSpread{ 0.f };
};

UCLASS()
class SHOOTERJAM_API AShooterHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;

private:
	FHUDPackage HUDPackage;

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax{ 60.f };

	void DrawCrosshair(UTexture2D* InCrosshair, const FVector2D InViewportCenter, const FVector2D Spread);

public:
	void SetHUDPackage(FHUDPackage& InHUDPackage);
};
