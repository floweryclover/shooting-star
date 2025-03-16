// Copyright 2025 ShootingStar. All Rights Reserved.


#include "LobbyNetworkComponent.h"

void ULobbyNetworkComponent::StartGame()
{
	AActor* const Owner = GetOwner();
	UWorld* const World = GetWorld();
	ensure(Owner);
	ensure(World);
	if (Owner == nullptr || World == nullptr || !Owner->HasAuthority())
	{
		return;
	}

	const FName CompetitiveLevelName = TEXT("/Game/Levels/Competitive");
	const FString Options = CompetitiveLevelName.ToString();
	World->ServerTravel(Options);
}

void ULobbyNetworkComponent::LeaveGame()
{
	UWorld* const World = GetWorld();
	ensure(GEngine);
	ensure(World);
	if (GEngine == nullptr || World == nullptr)
	{
		return;
	}

	APlayerController* const PlayerController = GEngine->GetFirstLocalPlayerController(World);
	const FName MainMenuLevelName = TEXT("/Game/Levels/MainMenu");
	const FString Options = MainMenuLevelName.ToString();
	PlayerController->ClientTravel(Options, TRAVEL_Absolute);
}



