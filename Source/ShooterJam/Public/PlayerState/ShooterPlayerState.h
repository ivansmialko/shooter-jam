// Made by smialko

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "ShooterPlayerState.generated.h"

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
	class AShooterCharacterController* PlayerController{ nullptr };

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

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

	virtual void UpdateScore(float InNewScore);
	virtual void UpdateDefeats(float InNewDefeats);

	void UpdateScoreHud();
	void UpdateDefeatsHud();

//public getters
public:
	FORCEINLINE int32 GetDefeats() const { return Defeats; }
};
