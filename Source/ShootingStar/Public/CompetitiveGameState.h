// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CompetitiveSystemComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CompetitiveGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIntChanged, int, Value);

DECLARE_DYNAMIC_DELEGATE_OneParam(FIntChangedHandler, int, Value);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFloatChanged, float, Value);

DECLARE_DYNAMIC_DELEGATE_OneParam(FFloatChangedHandler, float, Value);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPhaseChanged, ECompetitiveGamePhase, Value);

DECLARE_DYNAMIC_DELEGATE_OneParam(FPhaseChangedHandler, ECompetitiveGamePhase, Value);

UCLASS()
class SHOOTINGSTAR_API ACompetitiveGameState final : public AGameStateBase
{
	GENERATED_BODY()

public:
	ACompetitiveGameState();
	
	virtual void Tick(float DeltaSeconds) override;

	int GetBlueTeamRoundScore() const
	{
		return BlueTeamRoundScore;
	}

	int GetRedTeamRoundScore() const
	{
		return RedTeamRoundScore;
	}

	int GetBlueTeamGameScore() const
	{
		return BlueTeamGameScore;
	}

	int GetRedTeamGameScore() const
	{
		return RedTeamGameScore;
	}

	float GetPhaseTime() const
	{
		return PhaseTime;
	}

	ECompetitiveGamePhase GetPhase() const
	{
		return Phase;
	}

	void BindOnBlueTeamRoundScoreChanged(const FIntChangedHandler& Delegate)
	{
		OnBlueTeamRoundScoreChanged.Add(Delegate);
	}
	
	void BindOnRedTeamRoundScoreChanged(const FIntChangedHandler& Delegate)
	{
		OnRedTeamRoundScoreChanged.Add(Delegate);
	}

	void BindOnBlueTeamGameScoreChanged(const FIntChangedHandler& Delegate)
	{
		OnRedTeamGameScoreChanged.Add(Delegate);
	}

	void BindOnRedTeamGameScoreChanged(const FIntChangedHandler& Delegate)
	{
		OnBlueTeamGameScoreChanged.Add(Delegate);
	}

	void BindOnPhaseChanged(const FPhaseChangedHandler& Delegate)
	{
		OnPhaseChanged.Add(Delegate);
	}

	void BindOnPhaseTimeChanged(const FFloatChangedHandler& Delegate)
	{
		OnPhaseTimeChanged.Add(Delegate);
	}

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(ReplicatedUsing=OnRep_BlueTeamRoundScore, BlueprintReadOnly)
	int BlueTeamRoundScore;

	UPROPERTY(ReplicatedUsing=OnRep_RedTeamRoundScore, BlueprintReadOnly)
	int RedTeamRoundScore;

	UPROPERTY(ReplicatedUsing=OnRep_BlueTeamGameScore, BlueprintReadOnly)
	int BlueTeamGameScore;

	UPROPERTY(ReplicatedUsing=OnRep_RedTeamGameScore, BlueprintReadOnly)
	int RedTeamGameScore;

	UPROPERTY(ReplicatedUsing=OnRep_PhaseTime, BlueprintReadOnly)
	float PhaseTime;

	UPROPERTY(ReplicatedUsing=OnRep_Phase, BlueprintReadOnly)
	ECompetitiveGamePhase Phase;

	UPROPERTY(BlueprintAssignable)
	FIntChanged OnBlueTeamRoundScoreChanged;

	UPROPERTY(BlueprintAssignable)
	FIntChanged OnRedTeamRoundScoreChanged;

	UPROPERTY(BlueprintAssignable)
	FIntChanged OnBlueTeamGameScoreChanged;

	UPROPERTY(BlueprintAssignable)
	FIntChanged OnRedTeamGameScoreChanged;

	UPROPERTY(BlueprintAssignable)
	FPhaseChanged OnPhaseChanged;

	UPROPERTY(BlueprintAssignable)
	FFloatChanged OnPhaseTimeChanged;
	
private:

	UFUNCTION()
	void OnRep_BlueTeamRoundScore();

	UFUNCTION()
	void OnRep_RedTeamRoundScore();

	UFUNCTION()
	void OnRep_BlueTeamGameScore();

	UFUNCTION()
	void OnRep_RedTeamGameScore();

	UFUNCTION()
	void OnRep_PhaseTime();

	UFUNCTION()
	void OnRep_Phase();

	template <typename T>
	static bool AssignIfDifferent(T& InOutVariable, const T& InCompare)
	{
		if (InOutVariable != InCompare)
		{
			InOutVariable = InCompare;
			return true;
		}

		return false;
	}
};
