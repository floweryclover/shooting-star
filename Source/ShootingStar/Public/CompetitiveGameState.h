// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CompetitiveSystemComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CompetitiveGameState.generated.h"

UCLASS()
class SHOOTINGSTAR_API ACompetitiveGameState final : public AGameStateBase
{
	GENERATED_BODY()

public:
	ACompetitiveGameState();
	
	virtual void Tick(float DeltaSeconds) override;

	int GetBlueTeamKills() const
	{
		return BlueTeamKills;
	}

	int GetRedTeamKills() const
	{
		return RedTeamKills;
	}

	int GetBlueTeamWinRounds() const
	{
		return BlueTeamWinRounds;
	}

	int GetRedTeamWinRounds() const
	{
		return RedTeamWinRounds;
	}

	bool IsGoldenKillTime() const
	{
		return bIsGoldenKillTime;
	}

	float GetPhaseTime() const
	{
		return PhaseTime;
	}

	ECompetitiveGamePhase GetPhase() const
	{
		return Phase;
	}

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	int BlueTeamKills;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int RedTeamKills;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int BlueTeamWinRounds;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int RedTeamWinRounds;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float PhaseTime;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float RemainingGameTime;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsGoldenKillTime;

	UPROPERTY(Replicated, BlueprintReadOnly)
	ECompetitiveGamePhase Phase;
};
