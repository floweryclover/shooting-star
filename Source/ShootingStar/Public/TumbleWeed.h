// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MapObjectActor.h"
#include "TumbleWeed.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class SHOOTINGSTAR_API ATumbleWeed final : public AMapObjectActor
{
	GENERATED_BODY()
	
public:
    ATumbleWeed();
    
protected:
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<USphereComponent> SphereComponent;    	
};
