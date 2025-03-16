// Copyright 2025 ShootingStar. All Rights Reserved.


#include "JoinOrHostGameComponent.h"

void UJoinOrHostGameComponent::JoinGame(const FString& Host)
{
	UWorld* const World = GetWorld();
	ensure(World);
	if (!World)
	{
		return;
	}

	APlayerController* const PlayerController = World->GetFirstPlayerController();
	ensure(PlayerController);
	if (!PlayerController)
	{
		return;
	}

	PlayerController->ClientTravel(Host, TRAVEL_Absolute);
}

void UJoinOrHostGameComponent::HostGame()
{
	UWorld* const World = GetWorld();
	ensure(World);
	if (!World)
	{
		return;
	}

	const FName LobbyLevelName = TEXT("/Game/Levels/Lobby");
	const FString ServerOptions = LobbyLevelName.ToString() + "?listen";

	World->ServerTravel(ServerOptions);
}
