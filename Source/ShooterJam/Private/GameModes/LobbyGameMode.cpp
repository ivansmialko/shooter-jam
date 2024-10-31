// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int NumberOfPlayers{ GameState->PlayerArray.Num() };
	if (NumberOfPlayers == 2)
	{
		UWorld* World{ GetWorld() };
		if(!World)
			return;
		bUseSeamlessTravel = true;
		World->ServerTravel(FString("/Game/Maps/GameMap?listen"));
	}
}
