// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerListChanged);
DECLARE_DYNAMIC_DELEGATE(FPlayerListChangedHandler);

/**
 * 
 */
UCLASS()
class SHOOTINGSTAR_API ALobbyGameState final : public AGameStateBase
{
	GENERATED_BODY()

public:
	ALobbyGameState();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	const TArray<FString>& GetRedTeamPlayerNames() const
	{
		return RedTeamPlayerNames;
	}
	
	const TArray<FString>& GetBlueTeamPlayerNames() const
	{
		return BlueTeamPlayerNames;
	}

	void BindOnPlayerListChanged(const FPlayerListChangedHandler& Delegate)
	{
		OnPlayerListChanged.Add(Delegate);
	}

protected:
	UPROPERTY(BlueprintReadOnly)
	float UpdateInterval = 1.0f;

	UPROPERTY(BlueprintReadOnly)
	float UpdateElapsed;
	
	UPROPERTY(ReplicatedUsing=OnRep_PlayerNames, BlueprintReadOnly)
	TArray<FString> BlueTeamPlayerNames;
	
	UPROPERTY(ReplicatedUsing=OnRep_PlayerNames, BlueprintReadOnly)
	TArray<FString> RedTeamPlayerNames;

	UPROPERTY(BlueprintAssignable)
	FPlayerListChanged OnPlayerListChanged;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_PlayerNames();
};
