#include "SafeZoneActor.h"

void ASafeZoneActor::BeginPlay()
{
    Super::BeginPlay();
    
    if (GetLocalRole() != ROLE_Authority)
    {
        return;
    }

    SetRadiusByAlpha(0.0f);
}