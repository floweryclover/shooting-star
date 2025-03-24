// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitivePlayerController.h"
#include "TeamComponent.h"

ACompetitivePlayerController::ACompetitivePlayerController()
{
	TeamComponent = CreateDefaultSubobject<UTeamComponent>(TEXT("TeamComponent"));
}
