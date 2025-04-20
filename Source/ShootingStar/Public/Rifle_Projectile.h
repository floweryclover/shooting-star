// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "Rifle_Projectile.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGSTAR_API ARifle_Projectile : public AProjectileBase
{
	GENERATED_BODY()

public:
	ARifle_Projectile();


protected:
	virtual void BeginPlay() override;

private:

public:
	virtual void OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};
