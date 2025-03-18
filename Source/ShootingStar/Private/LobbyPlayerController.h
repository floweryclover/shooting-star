// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ALobbyPlayerController final : public APlayerController
{
	GENERATED_BODY()

public:
	ALobbyPlayerController();

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> LobbyUIClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> LobbyUI;
};
