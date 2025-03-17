// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitiveGameMode.h"
#include "CompetitivePlayerController.h"

ACompetitiveGameMode::ACompetitiveGameMode()
{
	PlayerControllerClass = ACompetitivePlayerController::StaticClass();
}