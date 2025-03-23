// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitiveGameMode.h"
#include "CompetitivePlayerController.h"
#include "CompetitiveGameState.h"
#include "CompetitiveSystemComponent.h"

ACompetitiveGameMode::ACompetitiveGameMode()
{
	PlayerControllerClass = ACompetitivePlayerController::StaticClass();
	GameStateClass = ACompetitiveGameState::StaticClass();
	CompetitiveSystemComponent = CreateDefaultSubobject<UCompetitiveSystemComponent>(TEXT("CompetitiveSystemComponent"));
}