// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "RocketLauncher.generated.h"

/**
 *
 */
class AProjectileBase;

UCLASS()
class SHOOTINGSTAR_API ARocketLauncher : public AGun
{
	GENERATED_BODY()

public:
	ARocketLauncher();

protected:
	virtual void BeginPlay() override;

private:

public:
	UPROPERTY(EditDefaultsOnly, Category = Rocket)
	TSubclassOf<AProjectileBase> RocketClass;

	virtual UClass* GetStaticClass() override;
	virtual AGun* SpawnToHand(APawn* owner, FVector loc, FRotator rot) override;
	virtual bool ProjectileFire(FVector loc, FRotator rot, FRotator bulletRot) override;
	virtual float GetFireCooldown() override { return 1.0f; }
};
