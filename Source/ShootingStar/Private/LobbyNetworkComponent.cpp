// Copyright 2025 ShootingStar. All Rights Reserved.


#include "LobbyNetworkComponent.h"

void ULobbyNetworkComponent::StartGame()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	const FName CompetitiveLevelName = TEXT("/Game/Levels/TestLevels/CompetitiveRoundSystemTest/CompetitiveRoundSystemTest");
	const FString Options = CompetitiveLevelName.ToString();
	GetWorld()->ServerTravel(Options);
}

void ULobbyNetworkComponent::LeaveGame()
{
	FWorldContext* const WorldContext = GetWorld()->GetGameInstance()->GetWorldContext();
	GEngine->BrowseToDefaultMap(*WorldContext);
}



