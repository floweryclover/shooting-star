// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WifiDirectDiscoverPC.generated.h"

/**
 * 
 */
UCLASS()
class WIFIDIRECT_API AWifiDirectDiscoverPC final : public APlayerController
{
	GENERATED_BODY()

public:
	AWifiDirectDiscoverPC();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> WifiDirectDiscoverUIClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> WifiDirectDiscoverUI;

	bool bOpenLevelRequested;
};
