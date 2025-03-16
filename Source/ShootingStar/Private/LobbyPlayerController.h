// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

class ULobbyNetworkComponent;
/**
 * 
 */
UCLASS()
class ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	ULobbyNetworkComponent* LobbyNetworkComponent;
	
	ALobbyPlayerController();

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> LobbyUIClass;

	UPROPERTY()
	UUserWidget* LobbyUI;
};
