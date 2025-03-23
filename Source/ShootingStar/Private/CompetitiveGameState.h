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
class ACompetitiveGameState final : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	int GetBlueTeamRoundScore() const
	{
		return BlueTeamRoundScore;
	}

	UFUNCTION(BlueprintCallable)
	int GetRedTeamRoundScore() const
	{
		return RedTeamRoundScore;
	}

	UFUNCTION(BlueprintCallable)
	int GetBlueTeamGameScore() const
	{
		return BlueTeamGameScore;
	}

	UFUNCTION(BlueprintCallable)
	int GetRedTeamGameScore() const
	{
		return RedTeamGameScore;
	}

	UFUNCTION(BlueprintCallable)
	float GetPhaseTime() const
	{
		return PhaseTime;
	}

	UFUNCTION(BlueprintCallable)
	ECompetitiveGamePhase GetPhase() const
	{
		return Phase;
	}

	UFUNCTION(BlueprintCallable)
	void BindOnBlueTeamRoundScoreChanged(const FIntChangedHandler& Delegate)
	{
		OnBlueTeamRoundScoreChanged.Add(Delegate);
	}
	
	UFUNCTION(BlueprintCallable)
	void BindOnRedTeamRoundScoreChanged(const FIntChangedHandler& Delegate)
	{
		OnRedTeamRoundScoreChanged.Add(Delegate);
	}

	UFUNCTION(BlueprintCallable)
	void BindOnBlueTeamGameScoreChanged(const FIntChangedHandler& Delegate)
	{
		OnRedTeamGameScoreChanged.Add(Delegate);
	}

	UFUNCTION(BlueprintCallable)
	void BindOnRedTeamGameScoreChanged(const FIntChangedHandler& Delegate)
	{
		OnBlueTeamGameScoreChanged.Add(Delegate);
	}

	UFUNCTION(BlueprintCallable)
	void BindOnPhaseChanged(const FPhaseChangedHandler& Delegate)
	{
		OnPhaseChanged.Add(Delegate);
	}

	UFUNCTION(BlueprintCallable)
	void BindOnPhaseTimeChanged(const FFloatChangedHandler& Delegate)
	{
		OnPhaseTimeChanged.Add(Delegate);
	}

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(ReplicatedUsing=OnRep_BlueTeamRoundScore, meta=(BlueprintGetter=GetBlueTeamRoundScore))
	int BlueTeamRoundScore;

	UPROPERTY(ReplicatedUsing=OnRep_RedTeamRoundScore, meta=(BlueprintGetter=GetRedTeamRoundScore))
	int RedTeamRoundScore;

	UPROPERTY(ReplicatedUsing=OnRep_BlueTeamGameScore, meta=(BlueprintGetter=GetBlueTeamGameScore))
	int BlueTeamGameScore;

	UPROPERTY(ReplicatedUsing=OnRep_RedTeamGameScore, meta=(BlueprintGetter=GetRedTeamGameScore))
	int RedTeamGameScore;

	UPROPERTY(ReplicatedUsing=OnRep_PhaseTime, meta=(BlueprintGetter=GetPhaseTime))
	float PhaseTime;

	UPROPERTY(ReplicatedUsing=OnRep_Phase, meta=(BlueprintGetter=GetPhase))
	ECompetitiveGamePhase Phase;

	UPROPERTY()
	FIntChanged OnBlueTeamRoundScoreChanged;

	UPROPERTY()
	FIntChanged OnRedTeamRoundScoreChanged;

	UPROPERTY()
	FIntChanged OnBlueTeamGameScoreChanged;

	UPROPERTY()
	FIntChanged OnRedTeamGameScoreChanged;

	UPROPERTY()
	FPhaseChanged OnPhaseChanged;

	UPROPERTY()
	FFloatChanged OnPhaseTimeChanged;

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
