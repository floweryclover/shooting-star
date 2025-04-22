// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

class ALobbyGameMode;
class ULobbyNetworkComponent;
class UTeamComponent;

/**
 * 
 */
UCLASS()
class SHOOTINGSTAR_API ALobbyPlayerController final : public APlayerController
{
	GENERATED_BODY()

public:
	ALobbyPlayerController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UTeamComponent* GetTeamComponent() const
	{
		return TeamComponent;
	}

	ULobbyNetworkComponent* GetLobbyNetworkComponent() const
	{
		return LobbyNetworkComponent;
	}

protected:
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<UUserWidget> LobbyUIClass;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UUserWidget> LobbyUI;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTeamComponent> TeamComponent;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULobbyNetworkComponent> LobbyNetworkComponent;

private:
	UPROPERTY()
	ALobbyGameMode* LobbyGameMode;
};
