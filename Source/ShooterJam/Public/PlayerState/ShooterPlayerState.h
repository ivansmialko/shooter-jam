// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "ShooterPlayerState.generated.h"
UENUM(BlueprintType)
enum class ETeamType : uint8
{
	ETT_NoTeam UMETA(DisplayName = "No team"),
	ETT_Red UMETA(DisplayName = "Red"),
	ETT_Blue UMETA(DisplayName = "Blue"),

	ETT_MAX UMETA(DisplayName = "DefaultMAX")
};

/**
 * 
 */
UCLASS()
class SHOOTERJAM_API AShooterPlayerState : public APlayerState
{
	GENERATED_BODY()

	//private members
private:
	class AShooterCharacter* Character{ nullptr };
	class AShooterCharacterController* ShooterCharacterController{ nullptr };

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

	UPROPERTY(ReplicatedUsing = OnRep_TeamType)
	ETeamType TeamType{ ETeamType::ETT_NoTeam };

//private functions
private:
	void CheckInitMembers();

//public functions
public:
	//~ Begin APlayerState Interface
	virtual void OnRep_Score() override;
	//~ End APlayerState Interface

	//~ Begin UObject Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface

	UFUNCTION()
	virtual void OnRep_Defeats();

	UFUNCTION()
	virtual void OnRep_TeamType(ETeamType InTeamType);

	virtual void UpdateScore(float InNewScore);
	virtual void UpdateDefeats(float InNewDefeats);

	void UpdateScoreHud();
	void UpdateDefeatsHud();

	void ChangeTeamType(ETeamType InTeamType);

//public getters
public:
	FORCEINLINE int32 GetDefeats() const { return Defeats; }
	FORCEINLINE ETeamType GetTeamType() const { return TeamType; }
};
