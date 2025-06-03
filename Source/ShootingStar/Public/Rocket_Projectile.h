// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Rocket_Projectile.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGSTAR_API ARocket_Projectile : public AProjectileBase
{
	GENERATED_BODY()
	
public:
	ARocket_Projectile();


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* ImpactNiagaraEffect;

private:

public:
	virtual void OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};
