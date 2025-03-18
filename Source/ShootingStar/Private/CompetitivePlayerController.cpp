// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitivePlayerController.h"
#include "TeamComponent.h"

ACompetitivePlayerController::ACompetitivePlayerController()
{
	CreateDefaultSubobject<UTeamComponent>(TEXT("TeamComponent"));
}
