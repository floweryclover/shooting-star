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

    // 보급품 상자가 열려있는지 확인하는 함수 추가
    UFUNCTION(BlueprintCallable)
    bool IsOpened() const { return bIsOpened; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY()
    bool bIsOpened;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FWeaponData StoredWeapon;
};
