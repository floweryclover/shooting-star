// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitiveGameMode.h"
#include "CompetitivePlayerController.h"
#include "CompetitiveGameState.h"

ACompetitiveGameMode::ACompetitiveGameMode()
{
	PlayerControllerClass = ACompetitivePlayerController::StaticClass();
	GameStateClass = ACompetitiveGameState::StaticClass();
}