// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGSTAR_API AMainMenuPlayerController final : public APlayerController
{
	GENERATED_BODY()

public:
	AMainMenuPlayerController();
	
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> MainMenuUIClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> MainMenuUI;
};
