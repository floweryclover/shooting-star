// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SupplyIndicatorUI.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGSTAR_API USupplyIndicatorUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Init_SupplyPos(FVector WorldPos);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool IsDestorySupply(FVector WorldPos);
};
