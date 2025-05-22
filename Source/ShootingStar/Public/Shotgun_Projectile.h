// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "Shotgun_Projectile.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGSTAR_API AShotgun_Projectile : public AProjectileBase
{
	GENERATED_BODY()

public:
	AShotgun_Projectile();

	static constexpr double FullDamageDistance = 500.0f;
	static constexpr double MinimumDamageDistance = 1000.0f;

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	float OriginalDamage;
};
