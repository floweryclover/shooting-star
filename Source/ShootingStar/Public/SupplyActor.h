// Copyright 2025 ShootingStar. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponData.h"
#include "SupplyActor.generated.h"

UCLASS()
class SHOOTINGSTAR_API ASupplyActor : public AActor
{
    GENERATED_BODY()

public:
    ASupplyActor();

    UFUNCTION(BlueprintCallable)
    void PlayOpeningAnimation();

    UFUNCTION(BlueprintImplementableEvent)
    void PlayOpenTimeline();

    UFUNCTION(BlueprintCallable)
    const FWeaponData& GetStoredWeapon() const { return StoredWeapon; }

    UFUNCTION(BlueprintCallable)
    bool IsOpened() const { return bIsOpened; }

protected:
    UPROPERTY()
    bool bIsOpened;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FWeaponData StoredWeapon;
};
