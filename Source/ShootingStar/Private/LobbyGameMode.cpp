// Copyright 2025 ShootingStar. All Rights Reserved.


#include "LobbyGameMode.h"
#include "LobbyPlayerController.h"

ALobbyGameMode::ALobbyGameMode()
{
	PlayerControllerClass = ALobbyPlayerController::StaticClass();
}
