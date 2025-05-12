// Copyright 2025 ShootingStar. All Rights Reserved.


#include "SupplyActor.h"
#include "CompetitiveGameMode.h"
#include "CompetitiveSystemComponent.h"
#include "Components/StaticMeshComponent.h"

ASupplyActor::ASupplyActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bIsOpened = false;

    Tags.Add(FName("Supply"));

    StoredWeapon.WeaponName = FText::FromString("RocketLauncher");
    StoredWeapon.NeedResourceCounts = 0;
    for (int32 i = 0; i < static_cast<int32>(EResourceType::End); ++i)
        StoredWeapon.UsedResourceCounts.Emplace(1);
}

void ASupplyActor::PlayOpeningAnimation()
{
    if (!bIsOpened)
    {
        bIsOpened = true;
        PlayOpenTimeline();
        UE_LOG(LogTemp, Log, TEXT("Playing Supply Open Animation"));
        
        if (ACompetitiveGameMode* GameMode = Cast<ACompetitiveGameMode>(GetWorld()->GetAuthGameMode()))
        {
            if (UCompetitiveSystemComponent* CompSystem = GameMode->GetCompetitiveSystemComponent())
                CompSystem->OnSupplyOpened.Broadcast(GetActorLocation());
        }
    }
}
