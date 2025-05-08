// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "Shotgun.generated.h"

/**
 *
 */
UCLASS()
class SHOOTINGSTAR_API AShotgun : public AGun
{
	GENERATED_BODY()

public:
	AShotgun();

protected:
	virtual void BeginPlay() override;

private:

public:
	virtual UClass* GetStaticClass() override;
	virtual AGun* SpawnToHand(APawn* owner, FVector loc, FRotator rot) override;
	virtual void ProjectileFire(FVector loc, FRotator rot, FRotator bulletRot) override;
};
