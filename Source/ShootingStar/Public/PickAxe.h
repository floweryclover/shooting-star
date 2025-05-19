// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Knife.h"
#include "PickAxe.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGSTAR_API APickAxe : public AKnife
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickAxe();

	virtual void OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult) override;
	
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
