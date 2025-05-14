// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SupplyIndicatorUI.generated.h"

class ASupplyActor;
/**
 * 
 */
UCLASS()
class SHOOTINGSTAR_API USupplyIndicatorUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitSupply(ASupplyActor* const SupplyActor);

	ASupplyActor* GetTargetSupplyActor() const
	{
		return TargetSupplyActor;
	}

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ASupplyActor> TargetSupplyActor;
};
